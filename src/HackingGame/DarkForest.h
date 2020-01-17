#pragma once

#pragma region 头文件

// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// 资源头文件
#include "resource.h"
#pragma comment(lib, "Msimg32.lib") //图片相关
#pragma comment(lib, "winmm.lib")	//多媒体相关
#pragma comment(lib,"imm32.lib")	//输入法相关
#pragma comment(lib,"gdiplus.lib")	//GDI+相关

// TODO:  在此处引用程序需要的其他头文件
#include <math.h>
#include <time.h>
#include <imm.h>	   //输入法相关
#include <mmsystem.h>  //多媒体相关
#include <gdiplus.h>   //GDI+相关
using namespace Gdiplus;

#pragma endregion

#pragma region 宏定义

#define STAGE_START			0		//开始画面的ID
#define STAGE_SELECT		1		//关卡选择画面的ID
#define STAGE_HELPCONTENTS	2		//帮助-目录画面的ID
#define STAGE_HELPCONTROL	3		//帮助-控制画面的ID
#define STAGE_HELPHERO		4		//帮助-主角画面的ID
#define STAGE_HELPENEMY		5		//帮助-敌人画面的ID
#define STAGE_HELBARRIER	6		//帮助-障碍画面的ID
#define STAGE_HELPABOUT		7		//帮助-关于画面的ID
#define STAGE_PAUSE			8		//游戏暂停画面的ID
#define STAGE_VICTORY		9		//游戏胜利画面的ID
#define STAGE_DEFEAT		10		//游戏失败画面的ID
#define STAGE_GAME1			11		//第一个游戏关卡的ID
#define STAGE_GAME2 		12		//第二个游戏关卡的ID

#define TIMER_GAMETIMER				1	//游戏的默认计时器ID
#define TIMER_GAMETIMER_ELAPSE		10	//默认计时器刷新间隔的毫秒数

#define MAXHPHERO					5	//主角最大HP
#define MAXMPHERO					5	//主角最大MP
#define MAXHPBOSS					80	//Boss最大HP
#define MAXHPENEMY					1	//敌人最大HP
#define MAXHPBARRIER				20	//障碍物最大HP
#define PROTECT_TIME				200	//长时间防护罩持续帧数
#define SPROTECT_TIME				40	//短时间防护罩持续帧数

#define BGX_SIZE					1536//背景宽度尺寸
#define BGY_SIZE					864	//背景高度尺寸
#define FRAME_SIZE                  138 //角色图片尺寸
#define BULLET_SIZE					20	//子弹图片尺寸
#define TRAPIN_SIZE					400	//陷阱图片尺寸
#define BARRIER_SIZE				256 //障碍物图片尺寸

#define ANIMATION_FRAME				3	//动画间隔帧数
#define SHOT_NUMBER					20  //镜头循环数量

#pragma endregion

#pragma region 结构体声明


// 按钮结构体
struct Button
{
	int x1;				//左上x坐标
	int y1;				//左上y坐标
	int x2;				//右下x坐标
	int y2;				//右下y坐标
};

//物体结构体
struct Object
{
	int ID;			//属性编号
	int HP;			//状态参量
	int x;			//坐标x
	int y;			//坐标y
	double vx;		//速度x
	double vy;		//速度y
	int frame;		//当前帧数
};

//数据结构体
struct Date
{
	HBITMAP img;	//图片
	int	radius;		//半径
	double acc;		//加速度
	double maxspeed;//最大速度
	int fire;		//射击方式
	int drop;		//掉落物
};


#pragma endregion

#pragma region 函数声明


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 初始化游戏窗体函数
void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 定时器事件处理函数
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 键盘按下事件处理函数
void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 键盘松开事件处理函数
void KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 鼠标移动事件处理函数
void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 鼠标左键按下事件处理函数
void LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 鼠标左键松开事件处理函数
void LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 鼠标右键按下事件处理函数
void RButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 鼠标右键松开事件处理函数
void RButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

//创建函数
Object *Create(int ID, int HP, int x, int y);

//帧数函数
void FrameHero();
void FrameWing();
void FrameOther(Object *tgt);

//运动函数
void MoveHero();
void MoveWing();
void MoveOther(Object *tgt);

//射击函数
void FireHero(int type);
void FireWing(int type);
void FireEnemy(Object *tgt, int type);

//召唤函数
void Summoner();

//闪现函数
void Flash();

//动画创建函数
void CreateAnimation(int x, int y);

//碰撞检测函数
//右留空与屏幕，左留空与全体
bool CollisionDetection(Object *tgt1, Object *tgt2);

//重置游戏函数
void ResetGame(int GameID);

//随机函数
int Rand(int a, int b);

// 绘图函数
void Paint(HWND hWnd);

#pragma endregion