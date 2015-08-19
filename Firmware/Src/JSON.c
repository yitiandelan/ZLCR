#include "JSON.h"

const char * strpt;
const char defstr[] = "{}";

int json_GetIntItem(const char * keyword, int * num)
{
//  char *p, *p1;
  int n = strlen(keyword);
//  if(n)
//    for(;;)
//    {
//      p = strstr(strpt, keyword);
//      p1 = strchr(p, ',');
//      if(p && (p1>p) && (*(p-1)=='"') && (*(p+n)=='"') && (*(p+n+1)==':'))
//        
//    }
  return 0;
}

int json_GetStrItem(const char * keyword)
{
  
  return 0;
}

int json_GetFloatItem(const char * keyword)
{
  
  return 0;
}

int json_GetDoubleItem(const char * keyword)
{
  
  return 0;
}

int json_loads(const char * str)
{
  int n,i,num1,num2,num3;
  const char * pt = str;
  for(n=0;n<128;n++)
  {
    if(*pt == '\0')
    {
      pt = str;
      num1 = num2 = num3 = 0;
      for(i=0;i<n;i++)
      {
        if(*pt == '{') num1++;
        else if(*pt == '}') num1--;
        else if(*pt == '[') num2++;
        else if(*pt == ']') num2--;
        else if(*pt == '"') num3++;
        pt++;
      }
      if(num1|num2|(num3%2 == 1)) return 0;
      break;
    }
    else if(!isprint(*pt))
      return 0;
    pt++;
  }
  return n;
}

int json_dumps()
{
  return 0;
}
