#!python3

from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5 import QtWidgets
from PyQt5 import uic

import json
import math
import numpy as np
import serial
import serial.tools.list_ports as list_ports
import sys
import time

CALI_RESISTANCE = 250.0

def gen_serial_port_list():
  port_list = []
  for info in list_ports.comports():
    port, desc, hwid = info
    port_list.append((port, desc))
  port_list.sort()
  return port_list
# end gen_serial_port_list()

class zlcr_meas_ui(QtWidgets.QWidget):
    def on_btn_dev_conn(self):
        port_idx  = self.cmb_dev_serial.currentIndex()
        port_name = self.serial_ports[port_idx][0]
        port_baud = int(self.edt_dev_baud.text())
        port_to   = float(self.edt_dev_to.text())
        # print(port_name, port_baud, port_to)
        try:
            self.serial_handler = serial.Serial( \
                port_name, \
                baudrate = port_baud, \
                timeout  = port_to, \
                parity   = serial.PARITY_NONE, \
                stopbits = serial.STOPBITS_ONE)
            self.flag_connected = self.serial_handler.isOpen()
        except Exception as e:
            print("serial error:", e)
        self.btn_dev_conn.setEnabled(not self.flag_connected)
        self.btn_dev_disc.setEnabled(self.flag_connected)
        self.grp_meas.setEnabled(self.flag_connected)
        if self.flag_connected:
            self.timer_meas.start(0.25)
        return
    def on_btn_dev_disc(self):
        if self.flag_connected:
            self.timer_meas.stop()
            self.serial_handler.close()
            self.flag_connected = False
            self.btn_dev_disc.setEnabled(False)
            self.btn_dev_conn.setEnabled(True)
            self.grp_meas.setEnabled(False)
        return
    def on_btn_setfreq(self):
        if self.serial_handler.isOpen():
            freq = float(self.edt_dev_setfreq.text())
            cmd = "zlcr -f %.2f\n" % freq
            # print(cmd.strip())
            self.serial_handler.write(cmd.encode())
        return
    def on_timer_meas(self):
        if self.serial_handler.isOpen():
            self.serial_handler.flushInput()
            self.serial_handler.readline()
            r = self.serial_handler.readline()
            if len(r):
                s = r.decode().strip()
                # print(s)
                j = json.loads(s)
                freq, mag, phase = j["FREQ"], j["MAG"], j["PHASE"]
                res = mag * CALI_RESISTANCE
                phase_deg = phase * 180.0 / math.pi
                if   phase_deg > 15.0:
                    omega = 2.0 * math.pi * freq
                    ind = res * math.sin(phase) / omega
                    cap = 0.0
                    rs  = res * math.cos(phase)
                    self.lcd_cap.setEnabled(False)
                    self.lcd_ind.setEnabled(True)
                    self.lab_rs.setEnabled(True)
                elif phase_deg < -15.0:
                    omega = 2.0 * math.pi * freq
                    ind = 0.0
                    cap = -1.0 / (res * math.sin(phase) * omega)
                    rs  = res * math.cos(phase)
                    self.lcd_cap.setEnabled(True)
                    self.lcd_ind.setEnabled(False)
                    self.lab_rs.setEnabled(True)
                else:
                    ind = 0.0
                    cap = 0.0
                    rs  = 0.0
                    self.lcd_cap.setEnabled(False)
                    self.lcd_ind.setEnabled(False)
                    self.lab_rs.setEnabled(False)
                self.lcd_freq.display(freq)
                self.lcd_res.display(res)
                self.lcd_phase.display(phase_deg)
                self.lcd_cap.display(cap * 1000000000.0)
                self.lcd_ind.display(ind * 1000000.0)
                self.lab_rs.setText("%.1f"%rs)
        return
    def init_ui(self):
        self.serial_ports = gen_serial_port_list()
        for k in range(len(self.serial_ports)):
            item = self.serial_ports[k]
            self.cmb_dev_serial.addItem("%s - %s"%(item[0], item[1]))
        self.btn_dev_conn.clicked.connect(self.on_btn_dev_conn)
        self.btn_dev_disc.clicked.connect(self.on_btn_dev_disc)
        self.btn_setfreq.clicked.connect(self.on_btn_setfreq)
        self.timer_meas = QtCore.QTimer()
        self.timer_meas.timeout.connect(self.on_timer_meas)
        self.lcd_cap.setEnabled(False)
        self.lcd_ind.setEnabled(False)
        self.btn_dev_disc.setEnabled(False)
        self.grp_meas.setEnabled(False)
        return
    def __init__(self):
        super(zlcr_meas_ui, self).__init__()
        uic.loadUi("zlcr_meas.ui", self)
        self.init_ui()
        self.show()
        return
# end class zlcr_meas_ui

def main(argv):
  app = QtWidgets.QApplication(argv)
  window = zlcr_meas_ui()
  app.exec_()
  return 0
# end main()

if __name__ == '__main__':
  sys.exit(main(sys.argv))
# end if
