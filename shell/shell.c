#include "comm.h"
#include "ucos_ii.h"

extern OS_EVENT *SemLedOn;
extern OS_EVENT *SemBeepOn;
extern OS_EVENT *SemLedOff;
extern OS_EVENT *SemBeepOff;


#ifdef SHELL_ENABLE

// 帮助指令
void ShowHelp(INT8U ch,INT8U * s)
{
    int i;
    Print("%d\n",ch);
    Print(ch,"\n\rlxq's mini2440 ucosii program  \n\r");
    Print(ch,"\n\rShell Command Help Menu\n\r");
    for (i = 0;i < CmdCnt;i++) 
        Print(ch,"%8s ----- %s\n\r",cmd_list[i].cmd,cmd_list[i].help);
    Print(ch,"\n\r>");
}

// LED打开指令
void LedOn(INT8U ch,INT8U * s){
    Print("%d\n",ch);
    OSSemPost(SemLedOn);
    Print(ch,"\n\r>");

}

void LedOff(INT8U ch,INT8U * s){
    Print("%d\n",ch);
    OSSemPost(SemLedOff);
    Print(ch,"\n\r>");
}

void BeepOn(INT8U ch,INT8U * s){
    Print("%d\n",ch);
    OSSemPost(SemBeepOn);
    Print(ch,"\n\r>");
}

void BeepOff(INT8U ch,INT8U * s){
    Print("%d\n",ch);
    OSSemPost(SemBeepOff);
    Print(ch,"\n\r>");
}

// 打印系统信息指令
void SystemInfo(INT8U ch,INT8U *s)
{   
    // Print(s);
	INT16U i = 0;
    INT8U err1,err2;
    INT8U tmp[40];
    OS_STK_DATA SkData;/*Storage for task stack data*/
    OS_TCB task_data;
    OSCtxSwCtr = 0;
    Print(ch,"\n\rTask ID            Name      Switch      All Stack       Used Stack     Unused Stack");
    for (i = 0; i < 31; i++) 
	{
		err1 = OSTaskQuery(i, &task_data);
        err2 = OSTaskStkChk(i, &SkData);
        if (err1 == 0 && err2 == 0) 
		{
            OSTaskNameGet(i,(unsigned char *)&tmp[0],&err1);
            Print(ch,"\n\rTask:%2d %15s      %6d          %6d           %6d           %6d",i,tmp,task_data.OSTCBCtxSwCtr,SkData.OSFree + SkData.OSUsed,SkData.OSUsed,SkData.OSFree);
        }
    }
	Print(ch,"\n\rCPU Usage:%03d%%\n\r>",OSCPUUsage);
}

// shell初始化
void ShellInit(void)
{
	CmdCnt = 0;
	memset(cmd_list,0,sizeof(cmd_list_type));
	AddCmd((INT8U *)"help"	,ShowHelp		,(INT8U *)"system Help");
	AddCmd((INT8U *)"sysinfo"	,SystemInfo		,(INT8U *)"system Infomation");
    AddCmd((INT8U *)"ledon",LedOn,(INT8U *)"led on");
    AddCmd((INT8U *)"ledoff",LedOff,(INT8U *)"led off");
    AddCmd((INT8U *)"beepon",BeepOn,(INT8U *)"beep on");
    AddCmd((INT8U *)"beepoff",BeepOff,(INT8U *)"beep off");
}



#endif





