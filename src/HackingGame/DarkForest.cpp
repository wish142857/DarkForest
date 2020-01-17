#include "DarkForest.h"
using namespace std;

#pragma region 全局变量

#define MAX_LOADSTRING			100		

// 全局变量: 

int const WINDOW_TITLEBARHEIGHT = 32;						//标题栏高度
int const WINDOW_WIDTH = GetSystemMetrics(SM_CXSCREEN);		//游戏窗口宽度
int const WINDOW_HEIGHT = GetSystemMetrics(SM_CYSCREEN);	//游戏窗口高度
int const ZONESIZE = 2000;									//游戏区域大小
HINSTANCE hInst;											// 当前窗体实例
WCHAR szTitle[MAX_LOADSTRING];								// 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];						// 主窗口类名

Button button[20];					//按钮数据
Date date[20];						//游戏数据
Object *pHero = NULL;               //主角指针
Object *pWingL = NULL;				//左僚机指针
Object *pWingR = NULL;				//右僚机指针
Object *pBoss = NULL;				//Boss指针
Object *pEnemy[20] = { NULL };		//敌人指针
Object *pHBullet[100] = { NULL };	//主角子弹指针
Object *pEBullet[500] = { NULL };   //敌人子弹指针
Object *pTrap = NULL;				//陷阱指针
Object *pCBarrier[20] = { NULL };	//可破坏障碍指针
Object *pUBarrier[20] = { NULL };	//不可破坏障碍指针
Object *pAnimation[30] = { NULL };	//动画指针
int nEnemy = 0;						//敌人数量
int nHBullet = 0;					//主角子弹数量
int nEBullet = 0;					//敌人子弹数量
int nCBarrier = 0;					//可破坏障碍数量
int nUBarrier = 0;					//不可破坏障碍数量
int nAnimation = 0;					//动画数量

int MPHero = 0;						//主角魔力值

bool ProtectHero = false;			//主角防护罩
bool ProtectBoss = false;			//Boss防护罩
int WingFireMode = 0;				//僚机攻击模式 0-跟随  1-自动

HIMC g_hIMC = NULL;					//启用或禁用输入法相关

GdiplusStartupInput    m_Gdistart;  //GUI+相关
ULONG_PTR    m_GdiplusToken;		//GUI+相关

HBITMAP bmp_bg[15];					//场景背景图片
Image *png_Protect;					//防护罩图片
Image *png_Trap;					//陷阱图片
Image *png_Crash;					//爆炸动画
Image *png_HPHero[2];				//主角血量图片
Image *png_MPHero[2];				//主角魔力图片
Image *png_HPBoss[9];				//Boss血量图片

unsigned seed = 0;					//随机化种子

bool timerOn = false;				//计时器是否运行
int Time = 0;						//时间辅助参数
int T_ProtectHero = 0;				//主角防护罩时间
int T_ProtectBoss = 0;				//Boss防护罩时间

int bgN = 0; //当前镜头
int bgX[SHOT_NUMBER] = { 0 }; //镜头坐标x
int bgY[SHOT_NUMBER] = { 0 }; //镜头坐标y

int currentStage = 0; //当前场景状态
int currentGame = 0;  //当前游戏关卡

int mouseX = 0;
int mouseY = 0;
bool mouseDown = false;
bool keyUpDown = false;
bool keyDownDown = false;
bool keyLeftDown = false;
bool keyRightDown = false;

double const PI = acos(double(-1));		//圆周率
double const Root = sqrt(2) / 2.0;		//二分之根号二
double const c0 = PI / 12.0;			//十五度角
double const c1 = PI / 6.0;				//三十度角
double const c2 = PI / 4.0;				//四十五度
#pragma endregion

#pragma region 主函数

//程序入口点
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,                                     //当前应用程序的实例句柄
                     _In_opt_ HINSTANCE hPrevInstance,								//前一个实例的句柄
                     _In_ LPWSTR    lpCmdLine,										//命令行参数
                     _In_ int       nCmdShow)										//主窗口显示方式
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	
	
	// 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HACKINGGAME, szWindowClass, MAX_LOADSTRING);
	//注册窗口
	MyRegisterClass(hInstance);      

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HACKINGGAME));

   
	
    // 主消息循环: 
	MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg); //TtanslateMessage仅为那些由键盘驱动器映射为ASCII字符的键产生WM_CHAR消息
            DispatchMessage(&msg);  //DispatchMessage将消息指派给WinProc处理
        }
    }

    return (int) msg.wParam;


}

#pragma endregion

# pragma region 窗口函数

#pragma region 设计窗口类
//
//  函数: MyRegisterClass()
//
//  目的: 设计与注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{	
	//设计窗口类
	WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;									 //窗口的类样式     水平重画 垂直重画
    wcex.lpfnWndProc    = WndProc;													 //设定回调函数
    wcex.cbClsExtra     = 0;														 //设为0即可
    wcex.cbWndExtra     = 0;														 //设为0即可
    wcex.hInstance      = hInstance;												 //当前应用程序的实例句柄
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));           //窗口图标的句柄
    wcex.hCursor        = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR));		 //窗口光标的句柄
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);                                  //窗口背景画刷的句柄
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HACKINGGAME);						 //指向菜单的指针
    wcex.lpszClassName  = szWindowClass;											 //指向类名称的指针
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));		 //和窗口类关联的小图标

	//注册窗口类
	return RegisterClassExW(&wcex);
}
#pragma endregion

#pragma region 创建窗口类
//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 创建、显示与更新窗口类
//         将实例句柄存储在全局变量中
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中
   
   //创建窗口类
   HWND hWnd = CreateWindow(
	   szWindowClass,	//主窗口类名
	   szTitle,			//标题栏文本
	   WS_POPUP,         //WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, // 设置窗口样式，不可改变大小，不可最大化,
	   0,    //CW_USEDEFAULT,	//窗口相对于父级的X坐标 
	   0,	 //CW_USEDEFAULT,	//窗口相对于父级的Y坐标  
	   WINDOW_WIDTH,	//窗口的宽度
	   WINDOW_HEIGHT,	//窗口的高度  
	   nullptr, nullptr, hInstance, nullptr);	//父窗口，菜单，当前应用程序的实例句柄，附加数据

   if (!hWnd)									//检查是否创建成功
	   return FALSE;
   
   //显示窗口类
   ShowWindow(hWnd, nCmdShow);                  //第一个参数为窗口句柄，第二参数为窗口显示方式          
   
   //更新窗口类
   UpdateWindow(hWnd);										  

   return TRUE;
}
#pragma endregion

#pragma region 消息处理函数
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 消息处理
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)   //接受消息窗口的句柄，主消息值，副消息值，副消息值
{
    switch (message)
    {
	case WM_CREATE:
		// 初始化游戏窗体
		InitGame(hWnd, wParam, lParam);
		break;
	case WM_KEYDOWN:
		// 键盘按下事件
		KeyDown(hWnd, wParam, lParam);
		break;
	case WM_KEYUP:
		// 键盘松开事件
		KeyUp(hWnd, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		// 鼠标移动事件
		MouseMove(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		// 鼠标左键按下事件
		LButtonDown(hWnd, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		// 鼠标左键松开事件
		LButtonUp(hWnd, wParam, lParam);
		break;
	case WM_RBUTTONDOWN:
		// 鼠标右键按下事件
		RButtonDown(hWnd, wParam, lParam);
		break;
	case WM_RBUTTONUP:
		// 鼠标右键松开事件
		RButtonUp(hWnd, wParam, lParam);
		break;
	case WM_TIMER:
		// 定时器事件
		if(timerOn)
			TimerUpdate(hWnd, wParam, lParam);
		break;
    case WM_PAINT:
		// 绘图
		Paint(hWnd);
        break;
    case WM_DESTROY:
		//窗口关闭
		PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
#pragma endregion

#pragma endregion

#pragma region 事件处理函数

#pragma region 初始化事件处理函数
void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// 加载按钮数据
	button[1].x1 = (int)((850.0 / 1536.0) * WINDOW_WIDTH);
	button[1].y1 = (int)((345.0 / 864.0) * WINDOW_HEIGHT);
	button[1].x2 = (int)((1250.0 / 1536.0) * WINDOW_WIDTH);
	button[1].y2 = (int)((415.0 / 864.0) * WINDOW_HEIGHT);
	button[2].x1 = (int)((850.0 / 1536.0) * WINDOW_WIDTH);
	button[2].y1 = (int)((470.0 / 864.0) * WINDOW_HEIGHT);
	button[2].x2 = (int)((1250.0 / 1536.0) * WINDOW_WIDTH);
	button[2].y2 = (int)((540.0 / 864.0) * WINDOW_HEIGHT);
	button[3].x1 = (int)((850.0 / 1536.0) * WINDOW_WIDTH);
	button[3].y1 = (int)((600.0 / 864.0) * WINDOW_HEIGHT);
	button[3].x2 = (int)((1250.0 / 1536.0) * WINDOW_WIDTH);
	button[3].y2 = (int)((670.0 / 864.0) * WINDOW_HEIGHT);
	button[4].x1 = (int)((460.0 / 1536.0) * WINDOW_WIDTH);
	button[4].y1 = (int)((320.0 / 864.0) * WINDOW_HEIGHT);
	button[4].x2 = (int)((660.0 / 1536.0) * WINDOW_WIDTH);
	button[4].y2 = (int)((510.0 / 864.0) * WINDOW_HEIGHT);
	button[5].x1 = (int)((930.0 / 1536.0) * WINDOW_WIDTH);
	button[5].y1 = (int)((320.0 / 864.0) * WINDOW_HEIGHT);
	button[5].x2 = (int)((1130.0 / 1536.0) * WINDOW_WIDTH);
	button[5].y2 = (int)((510.0 / 864.0) * WINDOW_HEIGHT);
	button[6].x1 = (int)((1130.0 / 1536.0) * WINDOW_WIDTH);
	button[6].y1 = (int)((680.0 / 864.0) * WINDOW_HEIGHT);
	button[6].x2 = (int)((1330.0 / 1536.0) * WINDOW_WIDTH);
	button[6].y2 = (int)((730.0 / 864.0) * WINDOW_HEIGHT);
	button[7].x1 = (int)((755.0 / 1536.0) * WINDOW_WIDTH);
	button[7].y1 = (int)((330.0 / 864.0) * WINDOW_HEIGHT);
	button[7].x2 = (int)((1030.0 / 1536.0) * WINDOW_WIDTH);
	button[7].y2 = (int)((400.0 / 864.0) * WINDOW_HEIGHT);
	button[8].x1 = (int)((1070.0 / 1536.0) * WINDOW_WIDTH);
	button[8].y1 = (int)((330.0 / 864.0) * WINDOW_HEIGHT);
	button[8].x2 = (int)((1300.0 / 1536.0) * WINDOW_WIDTH);
	button[8].y2 = (int)((400.0 / 864.0) * WINDOW_HEIGHT);
	button[9].x1 = (int)((910.0 / 1536.0) * WINDOW_WIDTH);
	button[9].y1 = (int)((445.0 / 864.0) * WINDOW_HEIGHT);
	button[9].x2 = (int)((1145.0 / 1536.0) * WINDOW_WIDTH);
	button[9].y2 = (int)((510.0 / 864.0) * WINDOW_HEIGHT);
	button[10].x1 = (int)((750.0 / 1536.0) * WINDOW_WIDTH);
	button[10].y1 = (int)((545.0 / 864.0) * WINDOW_HEIGHT);
	button[10].x2 = (int)((1030.0 / 1536.0) * WINDOW_WIDTH);
	button[10].y2 = (int)((610.0 / 864.0) * WINDOW_HEIGHT);
	button[11].x1 = (int)((1070.0 / 1536.0) * WINDOW_WIDTH);
	button[11].y1 = (int)((545.0 / 864.0) * WINDOW_HEIGHT);
	button[11].x2 = (int)((1325.0 / 1536.0) * WINDOW_WIDTH);
	button[11].y2 = (int)((610.0 / 864.0) * WINDOW_HEIGHT);
	button[12].x1 = (int)((1130.0 / 1536.0) * WINDOW_WIDTH);
	button[12].y1 = (int)((680.0 / 864.0) * WINDOW_HEIGHT);
	button[12].x2 = (int)((1330.0 / 1536.0) * WINDOW_WIDTH);
	button[12].y2 = (int)((735.0 / 864.0) * WINDOW_HEIGHT);
	button[13].x1 = (int)((1130.0 / 1536.0) * WINDOW_WIDTH);
	button[13].y1 = (int)((680.0 / 864.0) * WINDOW_HEIGHT);
	button[13].x2 = (int)((1330.0 / 1536.0) * WINDOW_WIDTH);
	button[13].y2 = (int)((735.0 / 864.0) * WINDOW_HEIGHT);
	button[14].x1 = (int)((670.0 / 1536.0) * WINDOW_WIDTH);
	button[14].y1 = (int)((370.0 / 864.0) * WINDOW_HEIGHT);
	button[14].x2 = (int)((870.0 / 1536.0) * WINDOW_WIDTH);
	button[14].y2 = (int)((420.0 / 864.0) * WINDOW_HEIGHT);
	button[15].x1 = (int)((670.0 / 1536.0) * WINDOW_WIDTH);
	button[15].y1 = (int)((465.0 / 864.0) * WINDOW_HEIGHT);
	button[15].x2 = (int)((870.0 / 1536.0) * WINDOW_WIDTH);
	button[15].y2 = (int)((515.0 / 864.0) * WINDOW_HEIGHT);
	button[16].x1 = (int)((670.0 / 1536.0) * WINDOW_WIDTH);
	button[16].y1 = (int)((560.0 / 864.0) * WINDOW_HEIGHT);
	button[16].x2 = (int)((870.0 / 1536.0) * WINDOW_WIDTH);
	button[16].y2 = (int)((610.0 / 864.0) * WINDOW_HEIGHT);
	button[17].x1 = (int)((670.0 / 1536.0) * WINDOW_WIDTH);
	button[17].y1 = (int)((400.0 / 864.0) * WINDOW_HEIGHT);
	button[17].x2 = (int)((870.0 / 1536.0) * WINDOW_WIDTH);
	button[17].y2 = (int)((450.0 / 864.0) * WINDOW_HEIGHT);
	button[18].x1 = (int)((670.0 / 1536.0) * WINDOW_WIDTH);
	button[18].y1 = (int)((510.0 / 864.0) * WINDOW_HEIGHT);
	button[18].x2 = (int)((870.0 / 1536.0) * WINDOW_WIDTH);
	button[18].y2 = (int)((560.0 / 864.0) * WINDOW_HEIGHT);
	// 加载游戏数据
	// 0 主角
	date[0].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
	date[0].radius = 50;
	date[0].acc = 0.8;
	date[0].maxspeed = 6.0;
	date[0].fire = 0;
	date[0].drop = 0;
	// 1 僚机
	date[1].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP6));
	date[1].radius = 25;
	date[1].acc = 0.6;
	date[1].maxspeed = 10.0;
	date[1].fire = 0;
	date[1].drop = 0;
	// 2 BOSS
	date[2].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP7));
	date[2].radius = 50;
	date[2].acc = 0.3;
	date[2].maxspeed = 2;
	date[2].fire = 1;
	date[2].drop = 0;
	// 3 敌人小兵
	date[3].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP8));
	date[3].radius = 50;
	date[3].acc = 0.3;
	date[3].maxspeed = 3;
	date[3].fire = 0;
	date[3].drop = 0;
	// 4 主角普通子弹（黄圆）
	date[4].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(162));
	date[4].radius = 10;
	date[4].acc = 0;
	date[4].maxspeed = 10.0;
	date[4].fire = 0;
	date[4].drop = 0;
	// 5 主角特殊子弹（黄星）
	date[5].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(163));
	date[5].radius = 10;
	date[5].acc = 0;
	date[5].maxspeed = 5.0;
	date[5].fire = 0;
	date[5].drop = 0;
	// 6 敌人可被破坏普通子弹（绿圆）
	date[6].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP16));
	date[6].radius = 10;
	date[6].acc = 0;
	date[6].maxspeed = 5.0;
	date[6].fire = 0;
	date[6].drop = 0;
	// 7 敌人可被破坏特殊子弹（绿星）
	date[7].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(165));
	date[7].radius = 10;
	date[7].acc = 0;
	date[7].maxspeed = 5.0;
	date[7].fire = 0;
	date[7].drop = 0;
	// 8 敌人不可被破坏普通子弹（蓝圆）
	date[8].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(166));
	date[8].radius = 10;
	date[8].acc = 0;
	date[8].maxspeed = 5.0;
	date[8].fire = 0;
	date[8].drop = 0;
	// 9 敌人不可被破坏特殊子弹（蓝星）
	date[9].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(167));
	date[9].radius = 10;
	date[9].acc = 0;
	date[9].maxspeed = 5.0;
	date[9].fire = 0;
	date[9].drop = 0;
	// 10 敌人追踪子弹（红圆）
	date[10].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP15));
	date[10].radius = 10;
	date[10].acc = 0.1;
	date[10].maxspeed = 8.0;
	date[10].fire = 0;
	date[10].drop = 0;
	// 11 陷阱（恒星）
	date[11].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(171));
	date[11].radius = 150;
	date[11].acc = 0;
	date[11].maxspeed = 0;
	date[11].fire = 0;
	date[11].drop = 0;
	// 12 可破坏障碍物（卫星）
	date[12].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(172));
	date[12].radius = 50;
	date[12].acc = 0;
	date[12].maxspeed = 0;
	date[12].fire = 0;
	date[12].drop = 0;
	// 13 不可破坏障碍物（行星）
	date[13].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(173));
	date[13].radius = 100;
	date[13].acc = 0;
	date[13].maxspeed = 0;
	date[13].fire = 0;
	date[13].drop = 0;
	// 14 不可破坏障碍物（行星）
	date[14].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(174));
	date[14].radius = 100;
	date[14].acc = 0;
	date[14].maxspeed = 0;
	date[14].fire = 0;
	date[14].drop = 0;
	// 15 不可破坏障碍物（行星）
	date[15].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(175));
	date[15].radius = 100;
	date[15].acc = 0;
	date[15].maxspeed = 0;
	date[15].fire = 0;
	date[15].drop = 0;
	// 16 不可破坏障碍物（行星）
	date[16].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(176));
	date[16].radius = 100;
	date[16].acc = 0;
	date[16].maxspeed = 0;
	date[16].fire = 0;
	date[16].drop = 0;
	// 17 不可破坏障碍物（行星）
	date[17].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(177));
	date[17].radius = 100;
	date[17].acc = 0;
	date[17].maxspeed = 0;
	date[17].fire = 0;
	date[17].drop = 0;
	// 18 不可破坏障碍物（行星）
	date[18].img = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(178));
	date[18].radius = 100;
	date[18].acc = 0;
	date[18].maxspeed = 0;
	date[18].fire = 0;
	date[18].drop = 0;
	// 19 爆炸动画
	date[19].img = NULL;
	date[19].radius = 64;
	date[19].acc = 0;
	date[19].maxspeed = 0;
	date[19].fire = 0;
	date[19].drop = 0;
	//初始化GDI+
	GdiplusStartup(&m_GdiplusToken, &m_Gdistart, NULL);
	//加载图像资源
	bmp_bg[0] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(182));
	bmp_bg[1] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(183));
	bmp_bg[2] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(184));
	bmp_bg[3] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(185));
	bmp_bg[4] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(186));
	bmp_bg[5] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(187));
	bmp_bg[6] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(188));
	bmp_bg[7] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(189));
	bmp_bg[8] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(190));
	bmp_bg[9] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(191));
	bmp_bg[10] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(192));
	bmp_bg[11] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(195));
	bmp_bg[12] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(196));
	png_Protect = new Image(L"./res/Protect.png");
	png_Trap = new Image(L"./res/Sun.png");
	png_Crash = new Image(L"./res/Crash.png");
	png_HPHero[0] = new Image(L"./res/HP0.png");
	png_HPHero[1] = new Image(L"./res/HP1.png");
	png_MPHero[0] = new Image(L"./res/MP0.png");
	png_MPHero[1] = new Image(L"./res/MP1.png");
	png_HPBoss[1] = new Image(L"./res/1.png");
	png_HPBoss[2] = new Image(L"./res/2.png");
	png_HPBoss[3] = new Image(L"./res/3.png");
	png_HPBoss[4] = new Image(L"./res/4.png");
	png_HPBoss[5] = new Image(L"./res/5.png");
	png_HPBoss[6] = new Image(L"./res/6.png");
	png_HPBoss[7] = new Image(L"./res/7.png");
	png_HPBoss[8] = new Image(L"./res/8.png");
	//初始化主计时器
	SetTimer(hWnd, TIMER_GAMETIMER, TIMER_GAMETIMER_ELAPSE, NULL);
	timerOn = false;
	//初始化随机化种子
	srand((unsigned)time(NULL));
	seed = rand();

	//屏蔽输入法
	g_hIMC = ImmAssociateContext(hWnd, NULL);
	//启用输入法
	//ImmAssociateContext(hWnd, g_hIMC);

	//播放背景音乐
	PlaySound(L"./res/The_Dawn.wav", hInst, SND_ASYNC | SND_LOOP);



}
#pragma endregion

#pragma region 定时器事件处理函数
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
//
//#pragma region 结束判定
//	if (pBoss->HP <= 0)
//	{
//		timerOn = false;
//		currentStage = STAGE_VICTORY;
//		PlaySound(L"./res/Victory.wav", hInst, SND_ASYNC);
//		InvalidateRect(hWnd, NULL, FALSE);
//		return;
//	}
//	if (pHero->HP <= 0)
//	{
//		timerOn = false;
//		currentStage = STAGE_DEFEAT;
//		PlaySound(L"./res/Defeat.wav", hInst, SND_ASYNC);
//		InvalidateRect(hWnd, NULL, FALSE);
//		return;
//	}
//#pragma endregion
//
#pragma region 主角更新
	int i = 0, j = 0, f = 0, temp = 0;
	Object *t = NULL;
	if (ProtectHero)
	{
		T_ProtectHero--;
		if (T_ProtectHero < 0)
			ProtectHero = false;
	}
	FrameHero();
	MoveHero();
	if (mouseDown && !(Time % 20))
		FireHero(date[0].fire);
#pragma endregion

#pragma region 僚机更新	
	FrameWing();
	MoveWing();
	if (WingFireMode)
		if (!(Time % 20))
			FireWing(date[1].fire);
#pragma endregion
//
//#pragma region Boss更新
//	if (ProtectBoss)
//	{
//		T_ProtectBoss--;
//		if (T_ProtectBoss < 0)
//			ProtectBoss = false;
//	}
//	FrameOther(pBoss);
//	MoveOther(pBoss);
//	// 运动阻碍
//	if (CollisionDetection(NULL, pBoss))
//	{
//		pBoss->x -= (int)(pBoss->vx);
//		pBoss->y -= (int)(pBoss->vy);
//		pBoss->vx = 0;
//		pBoss->vy = 0;
//	}
//	// 射击
//	if (!(Time % 100))
//	{
//		Summoner();
//		FireEnemy(pBoss, date[2].fire);		
//		if (pBoss->HP < 40)
//		{
//			if (currentStage == STAGE_GAME1)
//				FireEnemy(pBoss, 4);
//			else if (currentStage == STAGE_GAME2)
//				FireEnemy(pBoss, 5);
//		}
//	}
//#pragma endregion
//
//#pragma region 敌人更新
//	for (i = 1; i <= nEnemy; i++)
//	{
//		FrameOther(pEnemy[i]);
//		if (((pHero->x - pEnemy[i]->x)*(pHero->x - pEnemy[i]->x) + (pHero->y - pEnemy[i]->y) * (pHero->y - pEnemy[i]->y)) > 5000)
//			MoveOther(pEnemy[i]);
//		// 运动阻碍
//		if (CollisionDetection(NULL, pEnemy[i]))
//		{
//			pEnemy[i]->x -= (int)(pEnemy[i]->vx);
//			pEnemy[i]->y -= (int)(pEnemy[i]->vy);
//			pEnemy[i]->vx = 0;
//			pEnemy[i]->vy = 0;
//		}
//		// 射击
//		if (!(Time % 100))
//		{
//			if (CollisionDetection(pEnemy[i], NULL))
//				FireEnemy(pEnemy[i], date[3].fire);
//		}
//	}
//#pragma endregion
//
//#pragma region 子弹更新
//	//主角子弹更新
//	i = 1;
//	while (i <= nHBullet)
//	{
//		//与屏幕碰撞检测
//		if (CollisionDetection(pHBullet[i], NULL))
//			MoveOther(pHBullet[i]);
//		else
//		{
//			free(pHBullet[i]);
//			pHBullet[i] = pHBullet[nHBullet];
//			nHBullet--; continue;
//		}
//		i++;
//	}
//	//敌人子弹更新
//	i = 1;
//	while (i <= nEBullet)
//	{
//		//与屏幕碰撞检测
//		if (CollisionDetection(pEBullet[i], NULL))
//		{
//			if (date[pEBullet[i]->ID].acc > 0)
//				FrameOther(pEBullet[i]);
//			MoveOther(pEBullet[i]);
//		}
//		else
//		{
//			free(pEBullet[i]);
//			pEBullet[i] = pEBullet[nEBullet];
//			nEBullet--; continue;
//		}
//		i++;
//	}
//#pragma endregion	
//
//#pragma region 子弹-主角碰撞检测
//	i = 1;
//	while (i <= nEBullet)
//	{
//		if (CollisionDetection(pHero, pEBullet[i]))
//		{
//
//			free(pEBullet[i]);
//			pEBullet[i] = pEBullet[nEBullet];
//			nEBullet--;
//			if (!ProtectHero)
//			{
//				pHero->HP--;
//				T_ProtectHero = PROTECT_TIME;
//				ProtectHero = true;
//			}
//			continue;
//		}
//		i++;
//	}
//#pragma endregion
//
//#pragma region 子弹-Boss碰撞检测
//	i = 1;
//	while (i <= nHBullet)
//	{
//		if (CollisionDetection(pBoss, pHBullet[i]))
//		{
//			//处理Boss
//			if (!ProtectBoss)
//			{
//				pBoss->HP--;
//			}
//			//处理主角子弹
//			free(pHBullet[i]);
//			pHBullet[i] = pHBullet[nHBullet];
//			nHBullet--;	
//			continue;
//		}
//		i++;
//	}
//#pragma endregion
//
//#pragma region 子弹-敌人碰撞检测
//	i = 1;
//	while (i <= nHBullet)
//	{
//		temp = 0;
//		for (j = 1; j <= nEnemy; j++)
//			if (CollisionDetection(pHBullet[i], pEnemy[j]))
//			{
//				temp = j; break;
//			}
//		if (temp)
//		{
//			//处理子弹
//			if (pHBullet[i]->ID != 5)
//			{
//				free(pHBullet[i]);
//				pHBullet[i] = pHBullet[nHBullet];
//				nHBullet--;
//			}
//			//处理敌人
//			CreateAnimation(pEnemy[temp]->x, pEnemy[temp]->y);
//			free(pEnemy[temp]);
//			pEnemy[temp] = pEnemy[nEnemy];
//			nEnemy--;
//			continue;
//		}
//		i++;
//	}
//#pragma endregion
//
//#pragma region 子弹-障碍物碰撞检测
//	//特殊检测，优化速度	
//	//子弹-陷阱
//	if (CollisionDetection(pTrap, NULL))
//	{
//		i = 1;
//		while (i <= nHBullet)
//		{
//			if (CollisionDetection(pTrap, pHBullet[i]))
//			{
//				free(pHBullet[i]);
//				pHBullet[i] = pHBullet[nHBullet];
//				nHBullet--;
//				continue;
//			}
//			i++;
//		}
//		i = 1;
//		while (i <= nEBullet)
//		{
//			if (CollisionDetection(pTrap, pEBullet[i]))
//			{
//				free(pEBullet[i]);
//				pEBullet[i] = pEBullet[nEBullet];
//				nEBullet--;
//				continue;
//			}
//			i++;
//		}
//
//	}
//	//子弹-可破坏障碍物
//	i = 1;
//	while (i <= nCBarrier)
//	{
//		if (CollisionDetection(pCBarrier[i], NULL))
//		{
//			j = 1; temp = 0;
//			while (j <= nHBullet)
//			{
//				if (CollisionDetection(pCBarrier[i], pHBullet[j]))
//				{
//					free(pHBullet[j]);
//					pHBullet[j] = pHBullet[nHBullet];
//					nHBullet--;
//					pCBarrier[i]->HP--;
//					if (pCBarrier[i]->HP <= 0)
//					{
//						temp = 1;
//						break;
//					}
//					continue;
//				}
//				j++;
//			}
//			if (temp)
//			{
//				CreateAnimation(pCBarrier[i]->x, pCBarrier[i]->y);
//				free(pCBarrier[i]);
//				pCBarrier[i] = pCBarrier[nCBarrier];
//				nCBarrier--; continue;
//			}
//			else
//			{
//				j = 1;
//				while (j <= nEBullet)
//				{
//					if (CollisionDetection(pCBarrier[i], pEBullet[j]))
//					{
//						free(pEBullet[j]);
//						pEBullet[j] = pEBullet[nEBullet];
//						nEBullet--;
//						pCBarrier[i]->HP--;
//						if (pCBarrier[i]->HP <= 0)
//						{
//							temp = 1;
//							break;
//						}
//						continue;
//					}
//					j++;
//				}
//				if (temp)
//				{
//					CreateAnimation(pCBarrier[i]->x, pCBarrier[i]->y);
//					free(pCBarrier[i]);
//					pCBarrier[i] = pCBarrier[nCBarrier];
//					nCBarrier--; continue;
//				}
//			}
//		}
//		i++;
//	}
//	//子弹-不可破坏障碍物
//	for (i = 1; i <= nUBarrier; i++)
//	{
//		if (CollisionDetection(pUBarrier[i], NULL))
//		{
//			j = 1;
//			while (j <= nHBullet)
//			{
//				if (CollisionDetection(pUBarrier[i], pHBullet[j]))
//				{
//					free(pHBullet[j]);
//					pHBullet[j] = pHBullet[nHBullet];
//					nHBullet--; continue;
//				}
//				j++;
//			}
//			j = 1;
//			while (j <= nEBullet)
//			{
//				if (CollisionDetection(pUBarrier[i], pEBullet[j]))
//				{
//					free(pEBullet[j]);
//					pEBullet[j] = pEBullet[nEBullet];
//					nEBullet--; continue;
//				}
//				j++;
//			}
//		}
//	}
//
//#pragma endregion
//
//#pragma region 子弹-子弹碰撞
//	i = 1;
//	while (i <= nEBullet)
//	{
//		if (pEBullet[i]->HP)            //可破坏
//		{
//			temp = 0;
//			for (j = 1; j <= nHBullet; j++)
//				if (CollisionDetection(pEBullet[i], pHBullet[j]))
//				{
//					temp = j; break;
//				}
//			if (temp)
//			{
//				//处理敌人子弹
//				free(pEBullet[i]);
//				pEBullet[i] = pEBullet[nEBullet];
//				nEBullet--;
//				//处理主角子弹
//				if (pHBullet[temp]->ID != 5)
//				{
//					free(pHBullet[temp]);
//					pHBullet[temp] = pHBullet[nHBullet];
//					nHBullet--;
//				}
//				continue;
//			}
//			else
//				i++;
//		}
//		else
//			i++;
//	}
//#pragma endregion

#pragma region 魔力更新
	if (MPHero < 5)
		if (!(Time % 300))
			MPHero++;
#pragma endregion

#pragma region 时间更新
	Time++;
	if (Time>30000) Time %= 30000;
#pragma endregion

#pragma region 动画更新
	i = 1;
	while (i <= nAnimation)
	{
		//与屏幕碰撞检测
		if (CollisionDetection(pAnimation[i], NULL))
		{
			pAnimation[i]->HP++;
			if (pAnimation[i]->HP >= (ANIMATION_FRAME * 12))
			{
				free(pAnimation[i]);
				pAnimation[i] = pAnimation[nAnimation];
				nAnimation--; continue;
			}
		}
		else
		{
			free(pAnimation[i]);
			pAnimation[i] = pAnimation[nAnimation];
			nAnimation--; continue;
		}
		i++;
	}
#pragma endregion

#pragma region 镜头更新
	bgN++; bgN %= SHOT_NUMBER;
	temp = pHero->x - (WINDOW_WIDTH >> 1);
	if (temp < 0) temp = 0;
	else if (temp > ZONESIZE - WINDOW_WIDTH) temp = ZONESIZE - WINDOW_WIDTH;
	bgX[(bgN + SHOT_NUMBER - 1) % SHOT_NUMBER] = temp;
	temp = pHero->y - (WINDOW_HEIGHT >> 1);
	if (temp < 0) temp = 0;
	else if (temp > ZONESIZE - WINDOW_HEIGHT) temp = ZONESIZE - WINDOW_HEIGHT;
	bgY[(bgN + SHOT_NUMBER - 1) % SHOT_NUMBER] = temp;
	//刷新显示
	InvalidateRect(hWnd, NULL, FALSE);
#pragma endregion

}
#pragma endregion

#pragma region 键盘按下事件处理函数
void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_UP:
		keyUpDown = true;
		break;
	case VK_DOWN:
		keyDownDown = true;
		break;
	case VK_LEFT:
		keyLeftDown = true;
		break;
	case VK_RIGHT:
		keyRightDown = true;
		break;
	case 'W':
		keyUpDown = true;
		break;
	case 'S':
		keyDownDown = true;
		break;
	case 'A':
		keyLeftDown = true;
		break;
	case 'D':
		keyRightDown = true;
		break;
	case 'F':
		if (currentStage == STAGE_GAME1 || currentStage == STAGE_GAME2)
		{
			if (WingFireMode) WingFireMode = 0;
			else WingFireMode = 1;
		}
		break;
	case VK_RETURN:
		break;
	case VK_SPACE:
		if (currentStage == STAGE_GAME1 || currentStage == STAGE_GAME2)
		{
			if (MPHero > 0)
			{
				MPHero--;
				FireHero(1);
			}
		}
		break;
	case VK_ESCAPE:
		// 进入暂停界面
		if (currentStage == STAGE_PAUSE)
		{
			timerOn = true;
			currentStage = currentGame;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		else if (currentStage == STAGE_GAME1 || currentStage == STAGE_GAME2)
		{
			timerOn = false;
			currentStage = STAGE_PAUSE;
			InvalidateRect(hWnd, NULL, FALSE);
		}	
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region 键盘松开事件处理函数
void KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_UP:
		keyUpDown = false;
		break;
	case VK_DOWN:
		keyDownDown = false;
		break;
	case VK_LEFT:
		keyLeftDown = false;
		break;
	case VK_RIGHT:
		keyRightDown = false;
		break;
	case 'W':
		keyUpDown = false;
		break;
	case 'S':
		keyDownDown = false;
		break;
	case 'A':
		keyLeftDown = false;
		break;
	case 'D':
		keyRightDown = false;
		break;
	default:
		break;
	}
}
#pragma endregion

#pragma region 鼠标移动事件处理函数
void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = LOWORD(lParam);
	mouseY = HIWORD(lParam);
}
#pragma endregion

#pragma region 鼠标左键按下事件处理函数
void LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = LOWORD(lParam);
	mouseY = HIWORD(lParam);
	mouseDown = true;

	//按钮判定
	switch (currentStage)
	{
		case STAGE_START:
			if ((mouseX > button[1].x1) && (mouseX < button[1].x2) && (mouseY > button[1].y1) && (mouseY < button[1].y2))
			{
				currentStage = STAGE_SELECT;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[2].x1) && (mouseX < button[2].x2) && (mouseY > button[2].y1) && (mouseY < button[2].y2))
			{
				currentStage = STAGE_HELPCONTENTS;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[3].x1) && (mouseX < button[3].x2) && (mouseY > button[3].y1) && (mouseY < button[3].y2))
				PostQuitMessage(0);
			break;
		case STAGE_SELECT:
			if ((mouseX > button[4].x1) && (mouseX < button[4].x2) && (mouseY > button[4].y1) && (mouseY < button[4].y2))
			{
				currentStage = STAGE_GAME1;
				ResetGame(STAGE_GAME1);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[5].x1) && (mouseX < button[5].x2) && (mouseY > button[5].y1) && (mouseY < button[5].y2))
			{
				currentStage = STAGE_GAME2;
				ResetGame(STAGE_GAME2);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[6].x1) && (mouseX < button[6].x2) && (mouseY > button[6].y1) && (mouseY < button[6].y2))
			{
				currentStage = STAGE_START;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_HELPCONTENTS:
			if ((mouseX > button[7].x1) && (mouseX < button[7].x2) && (mouseY > button[7].y1) && (mouseY < button[7].y2))
			{
				currentStage = STAGE_HELPCONTROL;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[8].x1) && (mouseX < button[8].x2) && (mouseY > button[8].y1) && (mouseY < button[8].y2))
			{
				currentStage = STAGE_HELPHERO;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[9].x1) && (mouseX < button[9].x2) && (mouseY > button[9].y1) && (mouseY < button[9].y2))
			{
				currentStage = STAGE_HELPENEMY;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[10].x1) && (mouseX < button[10].x2) && (mouseY > button[10].y1) && (mouseY < button[10].y2))
			{
				currentStage = STAGE_HELBARRIER;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[11].x1) && (mouseX < button[11].x2) && (mouseY > button[11].y1) && (mouseY < button[11].y2))
			{
				currentStage = STAGE_HELPABOUT;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[12].x1) && (mouseX < button[12].x2) && (mouseY > button[12].y1) && (mouseY < button[12].y2))
			{
				currentStage = STAGE_START;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_HELPCONTROL:
			if ((mouseX > button[13].x1) && (mouseX < button[13].x2) && (mouseY > button[13].y1) && (mouseY < button[13].y2))
			{
				currentStage = STAGE_HELPCONTENTS;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_HELPHERO:
			if ((mouseX > button[13].x1) && (mouseX < button[13].x2) && (mouseY > button[13].y1) && (mouseY < button[13].y2))
			{
				currentStage = STAGE_HELPCONTENTS;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_HELPENEMY:
			if ((mouseX > button[13].x1) && (mouseX < button[13].x2) && (mouseY > button[13].y1) && (mouseY < button[13].y2))
			{
				currentStage = STAGE_HELPCONTENTS;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_HELBARRIER:
			if ((mouseX > button[13].x1) && (mouseX < button[13].x2) && (mouseY > button[13].y1) && (mouseY < button[13].y2))
			{
				currentStage = STAGE_HELPCONTENTS;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_HELPABOUT:
			if ((mouseX > button[13].x1) && (mouseX < button[13].x2) && (mouseY > button[13].y1) && (mouseY < button[13].y2))
			{
				currentStage = STAGE_HELPCONTENTS;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_PAUSE:
			if ((mouseX > button[14].x1) && (mouseX < button[14].x2) && (mouseY > button[14].y1) && (mouseY < button[14].y2))
			{
				timerOn = true;
				currentStage = currentGame;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[15].x1) && (mouseX < button[15].x2) && (mouseY > button[15].y1) && (mouseY < button[15].y2))
			{
				currentStage = currentGame;
				ResetGame(currentGame);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[16].x1) && (mouseX < button[16].x2) && (mouseY > button[16].y1) && (mouseY < button[16].y2))
			{
				currentStage = STAGE_START;
				PlaySound(L"./res/The_Dawn.wav", hInst, SND_ASYNC | SND_LOOP);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_VICTORY:
			if ((mouseX > button[17].x1) && (mouseX < button[17].x2) && (mouseY > button[17].y1) && (mouseY < button[17].y2))
			{
				currentStage = currentGame;
				ResetGame(currentGame);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[18].x1) && (mouseX < button[18].x2) && (mouseY > button[18].y1) && (mouseY < button[18].y2))
			{
				currentStage = STAGE_START;
				PlaySound(L"./res/The_Dawn.wav", hInst, SND_ASYNC | SND_LOOP);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_DEFEAT:
			if ((mouseX > button[17].x1) && (mouseX < button[17].x2) && (mouseY > button[17].y1) && (mouseY < button[17].y2))
			{
				currentStage = currentGame;
				ResetGame(currentGame);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if ((mouseX > button[18].x1) && (mouseX < button[18].x2) && (mouseY > button[18].y1) && (mouseY < button[18].y2))
			{
				currentStage = STAGE_START;
				PlaySound(L"./res/The_Dawn.wav", hInst, SND_ASYNC | SND_LOOP);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case STAGE_GAME1:
			break;
		case STAGE_GAME2:
			break;
	}
	return;
}
#pragma endregion

#pragma region 鼠标左键松开事件处理函数
void LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = LOWORD(lParam);
	mouseY = HIWORD(lParam);
	mouseDown = false;
	return;
}
#pragma endregion

#pragma region 鼠标右键按下事件处理函数
void RButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = LOWORD(lParam);
	mouseY = HIWORD(lParam);
	if (currentStage == 11 || currentStage == 12)
	{
		if (MPHero > 0)
		{
			Flash();
		}
	}
	return;
}
#pragma endregion

#pragma region 鼠标右键松开事件处理函数
void RButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	mouseX = LOWORD(lParam);
	mouseY = HIWORD(lParam);
	return;
}
#pragma endregion

#pragma endregion

#pragma region 创建函数
Object *Create(int ID, int HP, int x, int y)
{
	Object *tgt = (Object *)malloc(sizeof(Object));
	tgt->ID = ID;
	tgt->HP = HP;
	tgt->x = x;
	tgt->y = y;
	tgt->vx = 0;
	tgt->vy = 0;
	tgt->frame = 0;
	return tgt;
}
#pragma endregion

#pragma region 帧数函数

void FrameHero()
{
	int dirX = int(mouseX) + bgX[bgN] - pHero->x;
	int dirY = int(mouseY) + bgY[bgN] - pHero->y;
	double dirLen = sqrt(double(dirX * dirX + dirY * dirY));
	if (dirLen == 0) pHero->frame = 0;
	else
	{
		double cos = dirX / dirLen;
		double arc = acos(cos);
		if (dirY < 0) arc = 2 * PI - arc;
		pHero->frame = (int)((arc + c0) / c1);
		if (pHero->frame == 12) pHero->frame = 0;
	}
}

void FrameWing()
{
	if (WingFireMode)
	{
		int i = 0, temp = 0, minlen = 0;
		int dirX = 0, dirY = 0;
		double dirLen = 0, cosl = 0, arc = 0;
		Object *p = NULL;
		minlen = (pWingL->x - pBoss->x)*(pWingL->x - pBoss->x) + (pWingL->y - pBoss->y)*(pWingL->y - pBoss->y);
		p=pBoss;
		for (i = 1; i <= nEnemy; i++)
		{
			temp = (pWingL->x - pEnemy[i]->x)*(pWingL->x - pEnemy[i]->x) + (pWingL->y - pEnemy[i]->y)*(pWingL->y - pEnemy[i]->y);
			if (temp < minlen)
			{
				minlen = temp;
				p = pEnemy[i];
			}
		}
		dirX = p->x - pWingL->x;
		dirY = p->y - pWingL->y;
		dirLen = sqrt(double(dirX * dirX + dirY * dirY));
		if (dirLen == 0) pWingL->frame = 0;
		else
		{
			cosl = dirX / dirLen;
			arc = acos(cosl);
			if (dirY < 0) arc = 2 * PI - arc;
			pWingL->frame = (int)((arc + c0) / c1);
			if (pWingL->frame == 12) pWingL->frame = 0;
		}
	
		minlen = (pWingR->x - pBoss->x)*(pWingR->x - pBoss->x) + (pWingR->y - pBoss->y)*(pWingR->y - pBoss->y);
		p = pBoss;
		for (i = 1; i <= nEnemy; i++)
		{
			temp = (pWingR->x - pEnemy[i]->x)*(pWingR->x - pEnemy[i]->x) + (pWingR->y - pEnemy[i]->y)*(pWingR->y - pEnemy[i]->y);
			if (temp < minlen)
			{
				minlen = temp;
				p = pEnemy[i];
			}
		}
		dirX = p->x - pWingR->x;
		dirY = p->y - pWingR->y;
		dirLen = sqrt(double(dirX * dirX + dirY * dirY));
		if (dirLen == 0) pWingR->frame = 0;
		else
		{
			cosl = dirX / dirLen;
			arc = acos(cosl);
			if (dirY < 0) arc = 2 * PI - arc;
			pWingR->frame = (int)((arc + c0) / c1);
			if (pWingR->frame == 12) pWingR->frame = 0;
		}
	}
	else
	{
		pWingL->frame = pHero->frame;
		pWingR->frame = pHero->frame;
	}
	return;
}

void FrameOther(Object *tgt)
{
	int dirX = pHero->x - tgt->x;
	int dirY = pHero->y - tgt->y;
	double dirLen = sqrt(double(dirX * dirX + dirY * dirY));
	if (dirLen == 0) tgt->frame = 0;
	else
	{
		double cos = dirX / dirLen;
		double arc = acos(cos);
		if (dirY < 0) arc = 2 * PI - arc;
		tgt->frame = (int)((arc + c0) / c1);
		if (tgt->frame == 12) tgt->frame = 0;
	}
	return;
}
#pragma endregion

#pragma region 运动函数

void MoveHero()
{
	int temp = 0;
	if (keyLeftDown) temp -= 1;
	if (keyRightDown) temp += 1;
	if (keyUpDown) temp -= 3;
	if (keyDownDown) temp += 3;
	switch (temp)
	{
		//左上
	case -4:
		pHero->vx -= date[0].acc * Root;
		pHero->vy -= date[0].acc * Root;
		break;
		//正上
	case -3:
		pHero->vy -= date[0].acc;
		break;
		//右上
	case -2:
		pHero->vx += date[0].acc * Root;
		pHero->vy -= date[0].acc * Root;
		break;
		//正左
	case -1:
		pHero->vx -= date[0].acc;
		break;
		//正右
	case 1:
		pHero->vx += date[0].acc;
		break;
		//左下
	case 2:
		pHero->vx -= date[0].acc * Root;
		pHero->vy += date[0].acc * Root;
		break;
		//正下
	case 3:
		pHero->vy += date[0].acc;
		break;
		//右下
	case 4:
		pHero->vx += date[0].acc * Root;
		pHero->vy += date[0].acc * Root;
		break;
	default:break;
	}
	double rv = sqrt((pHero->vx) * (pHero->vx) + (pHero->vy) * (pHero->vy));
	//摩擦力模拟
	if (!temp && rv)
	{
		pHero->vx -= date[0].acc * (pHero->vx / rv);
		pHero->vy -= date[0].acc * (pHero->vy / rv);
	}
	//速度约束
	if (rv > date[pHero->ID].maxspeed)
	{
		pHero->vx = pHero->vx * date[0].maxspeed / rv;
		pHero->vy = pHero->vy * date[0].maxspeed / rv;
	}
	//位移改变
	pHero->x += (int)(pHero->vx);
	pHero->y += (int)(pHero->vy);
	//碰撞检测
	temp = 0;
	if (CollisionDetection(pHero, pTrap))
	{
		pHero->x -= (int)(pHero->vx);
		pHero->y -= (int)(pHero->vy);
		pHero->vx = 0;
		pHero->vy = 0;
		if (!ProtectHero)
		{
			pHero->HP--;
			T_ProtectHero = PROTECT_TIME;
			ProtectHero = true;
		}
	}
	else if (CollisionDetection(NULL, pHero))
	{
		pHero->x -= (int)(pHero->vx);
		pHero->y -= (int)(pHero->vy);
		pHero->vx = 0;
		pHero->vy = 0;
	}
	//位移约束
	if (pHero->x < 0) pHero->x = 0;
	else if (pHero->x > ZONESIZE) pHero->x = ZONESIZE;
	if (pHero->y < 0) pHero->y = 0;
	else if (pHero->y > ZONESIZE) pHero->y = ZONESIZE;
	return;
}

void MoveWing()
{
	int temp = 0;
	if (WingFireMode)
	{
		int dirX = pHero->x - pWingL->x;
		int dirY = pHero->y - pWingL->y;
		double dirLen = sqrt(double(dirX * dirX + dirY * dirY));
		double cosl = 0, arc = 0, rv = 0;
		if (dirLen)
		{
			cosl = dirX / dirLen;
			arc = acos(cosl);
			if (dirY < 0) arc = 2 * PI - arc;
			temp = (int)((arc + c0) / c1);
			if (temp == 12) temp = 0;
		}
		//速度改变
		pWingL->vx += date[pWingL->ID].acc * cos(temp * c1);
		pWingL->vy += date[pWingL->ID].acc * sin(temp * c1);
		rv = sqrt((pWingL->vx) * (pWingL->vx) + (pWingL->vy) * (pWingL->vy));
		//速度约束
		if (rv > date[pWingL->ID].maxspeed)
		{
			pWingL->vx = pWingL->vx * date[pWingL->ID].maxspeed / rv;
			pWingL->vy = pWingL->vy * date[pWingL->ID].maxspeed / rv;
		}
		//位移改变
		pWingL->x += (int)(pWingL->vx);
		pWingL->y += (int)(pWingL->vy);

		dirX = pHero->x - pWingR->x;
		dirY = pHero->y - pWingR->y;
		dirLen = sqrt(double(dirX * dirX + dirY * dirY));
		if (dirLen)
		{
			cosl = dirX / dirLen;
			arc = acos(cosl);
			if (dirY < 0) arc = 2 * PI - arc;
			temp = (int)((arc + c0) / c1);
			if (temp == 12) temp = 0;
		}
		//速度改变
		pWingR->vx += date[pWingR->ID].acc * cos(temp * c1);
		pWingR->vy += date[pWingR->ID].acc * sin(temp * c1);
		rv = sqrt((pWingR->vx) * (pWingR->vx) + (pWingR->vy) * (pWingR->vy));
		//速度约束
		if (rv > date[pWingR->ID].maxspeed)
		{
			pWingR->vx = pWingR->vx * date[pWingR->ID].maxspeed / rv;
			pWingR->vy = pWingR->vy * date[pWingR->ID].maxspeed / rv;
		}
		//位移改变
		pWingR->x += (int)(pWingR->vx);
		pWingR->y += (int)(pWingR->vy);
	}
	else
	{
		temp = (pHero->frame + 8) % 12;
		pWingL->x = pHero->x + int((FRAME_SIZE >> 1)  * cos(temp * c1));
		pWingL->y = pHero->y + int((FRAME_SIZE >> 1)  * sin(temp * c1));
		temp = (pHero->frame + 4) % 12;
		pWingR->x = pHero->x + int((FRAME_SIZE >> 1)  * cos(temp * c1));
		pWingR->y = pHero->y + int((FRAME_SIZE >> 1)  * sin(temp * c1));
	}	
	return;
}

void MoveOther(Object *tgt)
{
	//速度改变
	tgt->vx += date[tgt->ID].acc * cos(tgt->frame * c1);
	tgt->vy += date[tgt->ID].acc * sin(tgt->frame * c1);
	double rv = sqrt((tgt->vx) * (tgt->vx) + (tgt->vy) * (tgt->vy));
	//速度约束
	if (rv > date[tgt->ID].maxspeed)
	{
		tgt->vx = tgt->vx * date[tgt->ID].maxspeed / rv;
		tgt->vy = tgt->vy * date[tgt->ID].maxspeed / rv;
	}
	//位移改变
	tgt->x += (int)(tgt->vx);
	tgt->y += (int)(tgt->vy);
}
#pragma endregion

#pragma region 射击函数
void FireHero(int type)
{
	if (nHBullet >= 80) return;
	if (type == 0)
	{
		nHBullet++;
		pHBullet[nHBullet] = (Object *)malloc(sizeof(Object));
		pHBullet[nHBullet]->ID = 4;
		pHBullet[nHBullet]->HP = 0;
		pHBullet[nHBullet]->x = pHero->x;
		pHBullet[nHBullet]->y = pHero->y;
		pHBullet[nHBullet]->vx = date[pHBullet[nHBullet]->ID].maxspeed * cos(pHero->frame * c1);
		pHBullet[nHBullet]->vy = date[pHBullet[nHBullet]->ID].maxspeed * sin(pHero->frame * c1);
		pHBullet[nHBullet]->frame = pHero->frame;
		if (!WingFireMode)
		{
			nHBullet++;
			pHBullet[nHBullet] = (Object *)malloc(sizeof(Object));
			pHBullet[nHBullet]->ID = 4;
			pHBullet[nHBullet]->HP = 0;
			pHBullet[nHBullet]->x = pWingL->x;
			pHBullet[nHBullet]->y = pWingL->y;
			pHBullet[nHBullet]->vx = date[pHBullet[nHBullet]->ID].maxspeed * cos(pWingL->frame * c1);
			pHBullet[nHBullet]->vy = date[pHBullet[nHBullet]->ID].maxspeed * sin(pWingL->frame * c1);
			pHBullet[nHBullet]->frame = pWingL->frame;
			nHBullet++;
			pHBullet[nHBullet] = (Object *)malloc(sizeof(Object));
			pHBullet[nHBullet]->ID = 4;
			pHBullet[nHBullet]->HP = 0;
			pHBullet[nHBullet]->x = pWingR->x;
			pHBullet[nHBullet]->y = pWingR->y;
			pHBullet[nHBullet]->vx = date[pHBullet[nHBullet]->ID].maxspeed * cos(pWingR->frame * c1);
			pHBullet[nHBullet]->vy = date[pHBullet[nHBullet]->ID].maxspeed * sin(pWingR->frame * c1);
			pHBullet[nHBullet]->frame = pWingR->frame;
		}
	}
	else if (type == 1)
	{
		for (int i = 0; i < 12; i++)
		{
			nHBullet++;
			pHBullet[nHBullet] = (Object *)malloc(sizeof(Object));
			pHBullet[nHBullet]->ID = 5;
			pHBullet[nHBullet]->HP = 0;
			pHBullet[nHBullet]->x = pHero->x;
			pHBullet[nHBullet]->y = pHero->y;
			pHBullet[nHBullet]->vx = date[pHBullet[nHBullet]->ID].maxspeed * cos(i * c1);
			pHBullet[nHBullet]->vy = date[pHBullet[nHBullet]->ID].maxspeed * sin(i * c1);
			pHBullet[nHBullet]->frame = i;
		}
		if (!WingFireMode)
		{
			nHBullet++;
			pHBullet[nHBullet] = (Object *)malloc(sizeof(Object));
			pHBullet[nHBullet]->ID = 5;
			pHBullet[nHBullet]->HP = 0;
			pHBullet[nHBullet]->x = pWingL->x;
			pHBullet[nHBullet]->y = pWingL->y;
			pHBullet[nHBullet]->vx = date[pHBullet[nHBullet]->ID].maxspeed * cos(pWingL->frame * c1);
			pHBullet[nHBullet]->vy = date[pHBullet[nHBullet]->ID].maxspeed * sin(pWingL->frame * c1);
			pHBullet[nHBullet]->frame = pWingL->frame;
			nHBullet++;
			pHBullet[nHBullet] = (Object *)malloc(sizeof(Object));
			pHBullet[nHBullet]->ID = 5;
			pHBullet[nHBullet]->HP = 0;
			pHBullet[nHBullet]->x = pWingR->x;
			pHBullet[nHBullet]->y = pWingR->y;
			pHBullet[nHBullet]->vx = date[pHBullet[nHBullet]->ID].maxspeed * cos(pWingR->frame * c1);
			pHBullet[nHBullet]->vy = date[pHBullet[nHBullet]->ID].maxspeed * sin(pWingR->frame * c1);
			pHBullet[nHBullet]->frame = pWingR->frame;
		}
	}
	return;
}

void FireWing(int type)
{
	if (nHBullet >= 90) return;
	if (type == 0)
	{
		nHBullet++;
		pHBullet[nHBullet] = (Object *)malloc(sizeof(Object));
		pHBullet[nHBullet]->ID = 4;
		pHBullet[nHBullet]->HP = 0;
		pHBullet[nHBullet]->x = pWingL->x;
		pHBullet[nHBullet]->y = pWingL->y;
		pHBullet[nHBullet]->vx = date[pHBullet[nHBullet]->ID].maxspeed * cos(pWingL->frame * c1);
		pHBullet[nHBullet]->vy = date[pHBullet[nHBullet]->ID].maxspeed * sin(pWingL->frame * c1);
		pHBullet[nHBullet]->frame = pWingL->frame;
		nHBullet++;
		pHBullet[nHBullet] = (Object *)malloc(sizeof(Object));
		pHBullet[nHBullet]->ID = 4;
		pHBullet[nHBullet]->HP = 0;
		pHBullet[nHBullet]->x = pWingR->x;
		pHBullet[nHBullet]->y = pWingR->y;
		pHBullet[nHBullet]->vx = date[pHBullet[nHBullet]->ID].maxspeed * cos(pWingR->frame * c1);
		pHBullet[nHBullet]->vy = date[pHBullet[nHBullet]->ID].maxspeed * sin(pWingR->frame * c1);
		pHBullet[nHBullet]->frame = pWingR->frame;
	}
	return;
}

void FireEnemy(Object *tgt, int type)
{
	if (nEBullet >= 300) return;
	int temp = 0;
	if (type == 0)
	{
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(tgt->frame * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(tgt->frame * c1);
			pEBullet[nEBullet]->frame = tgt->frame;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(tgt->frame * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(tgt->frame * c1);
			pEBullet[nEBullet]->frame = tgt->frame;
		}
	}
	else if (type == 1)
	{
		temp = tgt->frame;
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		temp = tgt->frame + 1;
		if (temp > 11) temp -= 12;
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		temp = tgt->frame - 1;
		if (temp < 0) temp += 12;
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
	}
	else if (type == 2)
	{
		temp = tgt->frame;
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		temp = tgt->frame + 1;
		if (temp > 11) temp -= 12;
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		temp = tgt->frame + 2;
		if (temp > 11) temp -= 12;
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		temp = tgt->frame - 1;
		if (temp < 0) temp += 12;
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		temp = tgt->frame - 2;
		if (temp < 0) temp += 12;
		if (Rand(1, 100) < 60)
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 6;
			pEBullet[nEBullet]->HP = 1;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}
		else
		{
			nEBullet++;
			pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
			pEBullet[nEBullet]->ID = 8;
			pEBullet[nEBullet]->HP = 0;
			pEBullet[nEBullet]->x = tgt->x;
			pEBullet[nEBullet]->y = tgt->y;
			pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
			pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
			pEBullet[nEBullet]->frame = temp;
		}	
	}
	else if (type == 3)
	{
		for (int i = 0; i < 12; i++)
		{
			if (Rand(1, 100) < 60)
			{
				nEBullet++;
				pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
				pEBullet[nEBullet]->ID = 7;
				pEBullet[nEBullet]->HP = 1;
				pEBullet[nEBullet]->x = tgt->x;
				pEBullet[nEBullet]->y = tgt->y;
				pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(i * c1);
				pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(i * c1);
				pEBullet[nEBullet]->frame = i;
			}
			else
			{
				nEBullet++;
				pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
				pEBullet[nEBullet]->ID = 9;
				pEBullet[nEBullet]->HP = 0;
				pEBullet[nEBullet]->x = tgt->x;
				pEBullet[nEBullet]->y = tgt->y;
				pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(i * c1);
				pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(i * c1);
				pEBullet[nEBullet]->frame = i;
			}
		}
	}
	else if (type == 4)
	{
		nEBullet++;
		pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
		pEBullet[nEBullet]->ID = 10;
		pEBullet[nEBullet]->HP = 0;
		pEBullet[nEBullet]->x = tgt->x;
		pEBullet[nEBullet]->y = tgt->y;
		pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(tgt->frame * c1);
		pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(tgt->frame * c1);
		pEBullet[nEBullet]->frame = tgt->frame;
	}
	else if (type == 5)
	{
		nEBullet++;
		pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
		pEBullet[nEBullet]->ID = 10;
		pEBullet[nEBullet]->HP = 0;
		pEBullet[nEBullet]->x = tgt->x;
		pEBullet[nEBullet]->y = tgt->y;
		pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(tgt->frame * c1);
		pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(tgt->frame * c1);
		pEBullet[nEBullet]->frame = tgt->frame;

		temp = tgt->frame + 1;
		if (temp > 11) temp -= 12;
		nEBullet++;
		pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
		pEBullet[nEBullet]->ID = 10;
		pEBullet[nEBullet]->HP = 0;
		pEBullet[nEBullet]->x = tgt->x;
		pEBullet[nEBullet]->y = tgt->y;
		pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
		pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
		pEBullet[nEBullet]->frame = temp;

		temp = tgt->frame - 1;
		if (temp < 0) temp += 12;
		nEBullet++;
		pEBullet[nEBullet] = (Object *)malloc(sizeof(Object));
		pEBullet[nEBullet]->ID = 10;
		pEBullet[nEBullet]->HP = 0;
		pEBullet[nEBullet]->x = tgt->x;
		pEBullet[nEBullet]->y = tgt->y;
		pEBullet[nEBullet]->vx = date[pEBullet[nEBullet]->ID].maxspeed * cos(temp * c1);
		pEBullet[nEBullet]->vy = date[pEBullet[nEBullet]->ID].maxspeed * sin(temp * c1);
		pEBullet[nEBullet]->frame = temp;
	}
	return;
}

#pragma endregion

#pragma region 召唤函数
void Summoner()
{
	if (T_ProtectBoss < SPROTECT_TIME)
	{
		T_ProtectBoss = SPROTECT_TIME;
		ProtectBoss = true;
	}
	if (nEnemy > 15)
		return;	
	int temp = 0;
	temp = Rand(1, 4);
	nEnemy++;
	switch (temp)
	{
	case 1:
		pEnemy[nEnemy] = Create(3, 1, bgX[bgN], bgY[bgN] + Rand(1, WINDOW_HEIGHT));//左
		break;
	case 2:
		pEnemy[nEnemy] = Create(3, 1, bgX[bgN] + Rand(1, WINDOW_WIDTH), bgY[bgN] + WINDOW_HEIGHT);//下
		break;
	case 3:
		pEnemy[nEnemy] = Create(3, 1, bgX[bgN] + WINDOW_WIDTH, bgY[bgN] + Rand(1, WINDOW_HEIGHT));//右
	case 4:
		pEnemy[nEnemy] = Create(3, 1, bgX[bgN] + Rand(1, WINDOW_WIDTH), bgY[bgN]);//上
	}
	if (CollisionDetection(NULL, pEnemy[nEnemy]))
	{
		free(pEnemy[nEnemy]);
		nEnemy--;
	}

	return;
}
#pragma endregion

#pragma region 闪现函数
void Flash()
{
	int tempX = pHero->x;
	int tempY = pHero->y;
	pHero->x = bgX[bgN] + mouseX;
	pHero->y = bgY[bgN] + mouseY;
	if (CollisionDetection(NULL, pHero))
	{
		//失败
		pHero->x = tempX;
		pHero->y = tempY;
		return;
	}
	for (int i = 1; i <= nEnemy; i++)
		if (CollisionDetection(pHero, pEnemy[i]))
		{
			//失败
			pHero->x = tempX;
			pHero->y = tempY;
			return;
		}
	//成功
	MPHero--;
	pHero->vx = 0;
	pHero->vy = 0;
	if (T_ProtectHero < SPROTECT_TIME)
	{
		T_ProtectHero = SPROTECT_TIME;
		ProtectHero = true;
	}
	int temp = 0;
	for (int i = 0; i <= SHOT_NUMBER - 1; i++)
	{
		//temp = pHero->x - (WINDOW_WIDTH >> 1);
		temp = tempX + ((pHero->x - tempX) * i) / (SHOT_NUMBER - 1) - (WINDOW_WIDTH >> 1);
		if (temp < 0) temp = 0;
		else if (temp > ZONESIZE - WINDOW_WIDTH) temp = ZONESIZE - WINDOW_WIDTH;
		bgX[(bgN + i) % SHOT_NUMBER] = temp;
		
		//temp = pHero->y - (WINDOW_HEIGHT >> 1);
		temp = tempY + ((pHero->y - tempY) * i) / (SHOT_NUMBER - 1) - (WINDOW_HEIGHT >> 1);
		if (temp < 0) temp = 0;
		else if (temp > ZONESIZE - WINDOW_HEIGHT) temp = ZONESIZE - WINDOW_HEIGHT;
		bgY[(bgN + i) % SHOT_NUMBER] = temp;
	}

	return;
}
#pragma endregion

#pragma region 动画创建函数
void CreateAnimation(int x, int y)
{
	if (nAnimation > 20)
		return;
	nAnimation++;
	pAnimation[nAnimation] = Create(19, 0, x, y);
	return;
}
#pragma endregion

#pragma region 碰撞检测函数
bool CollisionDetection(Object *tgt1, Object *tgt2)
{
	if (tgt1 == tgt2) return false;		//排除自检索
	if (!tgt2)							//与屏幕检测,矩形
	{
		int a = bgX[bgN];
		int b = tgt1->x - date[tgt1->ID].radius;
		int min = a > b ? a : b;
		a += WINDOW_WIDTH;
		b = tgt1->x + date[tgt1->ID].radius;
		int max = a < b ? a : b;
		if (min >= max) return false;

		a = bgY[bgN];
		b = tgt1->y - date[tgt1->ID].radius;
		min = a > b ? a : b;
		a += WINDOW_HEIGHT;
		b = tgt1->y + date[tgt1->ID].radius;
		max = a < b ? a : b;
		if (min >= max) return false;
		return true;
	}
	else if (!tgt1)					//与全体（Hero Boss Trap CBarrier UBarrier）检测，圆形
	{
		if (CollisionDetection(tgt2, pHero)) return true;
		if (CollisionDetection(tgt2, pBoss)) return true;
		if (CollisionDetection(tgt2, pTrap)) return true;
		for (int i = 1; i <= nCBarrier; i++)
			if (CollisionDetection(tgt2, pCBarrier[i])) return true;
		for (int i = 1; i <= nUBarrier; i++)
			if (CollisionDetection(tgt2, pUBarrier[i])) return true;
		return false;
	}
	else								//物体间检测，圆形
	{
		int d = (tgt1->x - tgt2->x) * (tgt1->x - tgt2->x) + (tgt1->y - tgt2->y) * (tgt1->y - tgt2->y);
		if (d < (date[tgt1->ID].radius + date[tgt2->ID].radius) * (date[tgt1->ID].radius + date[tgt2->ID].radius))
			return true;
		else
			return false;
	}
}
#pragma endregion

#pragma region 重置游戏函数
void ResetGame(int GameID)
{
	
	currentGame = GameID;
	timerOn = true;
	//启动计时器
	//注意，一旦离开游戏场景，关闭计时器!!!!!!!!!
	//初始化镜头
	for (int i = 0; i < SHOT_NUMBER; i++)
	{
		bgX[i] = 0; bgY[i] = 0;
	}
	//初始化动画
	for (int i = 1; i <= nAnimation; i++)
		free(pAnimation[i]);
	nAnimation = 0;
	//初始化游戏数据
	if (GameID == STAGE_GAME1)
	{
		date[0].fire = 0;
		date[1].fire = 0;
		date[2].fire = 2;
		date[3].fire = 0;
		PlaySound(L"./res/Endeavors.wav", hInst, SND_ASYNC | SND_LOOP);
	}
	else if (GameID == STAGE_GAME2)
	{
		date[0].fire = 0;
		date[1].fire = 0;
		date[2].fire = 3;
		date[3].fire = 1;
		PlaySound(L"./res/inevitable_struggle.wav", hInst, SND_ASYNC | SND_LOOP);
	}	
	//初始化主角和僚机
	if (pHero) free(pHero);
	pHero = Create(0, MAXHPHERO, WINDOW_WIDTH >> 1, WINDOW_HEIGHT >> 1);
	MPHero = 0; WingFireMode = 0;
	T_ProtectHero = PROTECT_TIME;
	ProtectHero = true;
	if (pWingL) free(pWingL);
	pWingL = Create(1, 0, (WINDOW_WIDTH >> 1) - 30, WINDOW_HEIGHT >> 1);
	if (pWingR) free(pWingR);
	pWingR = Create(1, 0, (WINDOW_WIDTH >> 1) + 30, WINDOW_HEIGHT >> 1);
	//初始化Boss和敌人
	if (pBoss) free(pBoss);
	pBoss = Create(2, MAXHPBOSS, WINDOW_WIDTH >> 1, 1);
	T_ProtectBoss = PROTECT_TIME;
	ProtectBoss = true;
	for (int i = 1; i <= nEnemy; i++)
		free(pEnemy[i]);
	nEnemy = 0;
	//初始化陷阱和障碍物
	if (pTrap) free(pTrap);
	while (1)
	{
		pTrap = Create(11, 0, Rand(200, 1800), Rand(200, 1800));
		if (CollisionDetection(pTrap, pHero) || CollisionDetection(pTrap, pBoss))
			free(pTrap);
		else
			break;
	}
	for (int i = 1; i <= nCBarrier; i++)
		free(pCBarrier[i]);
	nCBarrier = 0;
	for (int i = 1; i <= nUBarrier; i++)
		free(pUBarrier[i]);
	nUBarrier = 0;
	/*for (int i = 1; i <= 10; i++)
	{
		nCBarrier++;
		pCBarrier[nCBarrier] = Create(12, MAXHPBARRIER, Rand(100, 1900), Rand(100, 1900));
		if (CollisionDetection(NULL, pCBarrier[nCBarrier]))
		{
			free(pCBarrier[nCBarrier]); nCBarrier--; continue;
		}
	}
	for (int i = 1; i <= 10; i++)
	{
		nUBarrier++;
		pUBarrier[nUBarrier] = Create(Rand(13, 18), 0, Rand(100, 1900), Rand(100, 1900));
		if (CollisionDetection(NULL, pUBarrier[nUBarrier]))
		{
			free(pUBarrier[nUBarrier]); nUBarrier--; continue;
		}
	}*/
	//初始化子弹
	for (int i = 1; i <= nHBullet; i++)
		free(pHBullet[i]);
	nHBullet = 0;
	for (int i = 1; i <= nEBullet; i++)
		free(pEBullet[i]);
	nEBullet = 0;
	//召唤第一批敌人
	for (int i = 1; i <= 10; i++)
		Summoner();
	return;
}
#pragma endregion

#pragma region 随机函数
int Rand(int a, int b)
{
	//生成a到b（含）的随机数
	srand(seed);
	seed = rand();
	return ((seed % (b - a + 1)) + a);
}
#pragma endregion

#pragma region 绘图函数
void Paint(HWND hWnd)
{
	int i = 0;
	PAINTSTRUCT ps;
	HDC hdc_window = BeginPaint(hWnd, &ps);  //显示器画布
	HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);  //大缓冲
	HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);  //小缓冲
	//初始化缓存
	HBITMAP	blankBmp = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(hdc_memBuffer, blankBmp);
	//初始化GDI+
	Graphics graph(hdc_memBuffer);
	graph.SetSmoothingMode(SmoothingModeAntiAlias);
	
	/*graph.RotateTransform(旋转角度 角度制, MatrixOrderAppend);
	graph.TranslateTransform((*iter).x, (*iter).y, MatrixOrderAppend);
	graph.DrawImage(cyanbullet, -BULLET_SIZE_X / 2, -BULLET_SIZE_Y / 2, BULLET_SIZE_X, BULLET_SIZE_Y);
	graph.ResetTransform();*/
	/*
	//png测试点
	Image* bg = new Image(L"./res/pro.png");						//载入指定路径的图像文件
	if (bg->GetHeight() != 0 && bg->GetWidth() != 0)			//判断图像文件是否存在
	graph.DrawImage(bg, 0, 0, 128, 128); //在指定图像控件的区域中绘制图像
	delete bg;
	*/
	//bmp测试点
	/*
	SelectObject(hdc_loadBmp, bmp_Trap);
	TransparentBlt
	(
		hdc_memBuffer, 1000 - bgX[bgN], 1000 - bgY[bgN],
		100, 1000,
		hdc_loadBmp, 0, 0, 100, 1000,
		RGB(0, 0, 0)
	);
	*/


	//分场景进行绘制
	if ((currentStage == STAGE_GAME1) || (currentStage == STAGE_GAME2))
	{
		//绘制游戏背景
		SelectObject(hdc_loadBmp, bmp_bg[currentStage]);
		BitBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadBmp, bgX[bgN], bgY[bgN], SRCCOPY);
		//// 绘制陷阱和障碍物到缓存
		//if (CollisionDetection(pTrap, NULL))
		//	graph.DrawImage(png_Trap, pTrap->x - bgX[bgN] - (TRAPIN_SIZE >> 1), pTrap->y - bgY[bgN] - (TRAPIN_SIZE >> 1), TRAPIN_SIZE, TRAPIN_SIZE);
		//for (i = 1; i <= nCBarrier; i++)
		//	if (CollisionDetection(pCBarrier[i], NULL))
		//	{
		//		SelectObject(hdc_loadBmp, date[pCBarrier[i]->ID].img);
		//		TransparentBlt
		//		(
		//			hdc_memBuffer, pCBarrier[i]->x - bgX[bgN] - date[pCBarrier[i]->ID].radius, pCBarrier[i]->y - bgY[bgN] - date[pCBarrier[i]->ID].radius,
		//			date[pCBarrier[i]->ID].radius << 1, date[pCBarrier[i]->ID].radius << 1,
		//			hdc_loadBmp, 0, 0, BARRIER_SIZE, BARRIER_SIZE,
		//			RGB(0, 0, 0)
		//		);

		//	}
		//for (i = 1; i <= nUBarrier; i++)
		//	if (CollisionDetection(pUBarrier[i], NULL))
		//	{
		//		SelectObject(hdc_loadBmp, date[pUBarrier[i]->ID].img);
		//		TransparentBlt
		//		(
		//			hdc_memBuffer, pUBarrier[i]->x - bgX[bgN] - date[pUBarrier[i]->ID].radius, pUBarrier[i]->y - bgY[bgN] - date[pUBarrier[i]->ID].radius,
		//			date[pUBarrier[i]->ID].radius << 1, date[pUBarrier[i]->ID].radius << 1,
		//			hdc_loadBmp, 0, 0, BARRIER_SIZE, BARRIER_SIZE,
		//			RGB(0, 0, 0)
		//		);
		//	}

		// 绘制主角到缓存
		if (ProtectHero)
			graph.DrawImage(png_Protect, pHero->x - bgX[bgN] - 64, pHero->y - bgY[bgN] - 64, 128, 128);
		SelectObject(hdc_loadBmp, date[0].img);
		TransparentBlt
		(
			hdc_memBuffer, pHero->x - bgX[bgN] - (FRAME_SIZE >> 1), pHero->y - bgY[bgN] - (FRAME_SIZE >> 1),
			FRAME_SIZE, FRAME_SIZE,
			hdc_loadBmp, 0, FRAME_SIZE * pHero->frame, FRAME_SIZE, FRAME_SIZE,
			RGB(0, 0, 0)
		);

		// 绘制僚机到缓存
		SelectObject(hdc_loadBmp, date[1].img);
		TransparentBlt
		(
			hdc_memBuffer, pWingL->x - bgX[bgN] - (FRAME_SIZE >> 2), pWingL->y - bgY[bgN] - (FRAME_SIZE >> 2),
			FRAME_SIZE >> 1, FRAME_SIZE >> 1,
			hdc_loadBmp, 0, FRAME_SIZE * pWingL->frame, FRAME_SIZE, FRAME_SIZE,
			RGB(0, 0, 0)
		);

		SelectObject(hdc_loadBmp, date[1].img);
		TransparentBlt
		(
			hdc_memBuffer, pWingR->x - bgX[bgN] - (FRAME_SIZE >> 2), pWingR->y - bgY[bgN] - (FRAME_SIZE >> 2),
			FRAME_SIZE >> 1, FRAME_SIZE >> 1,
			hdc_loadBmp, 0, FRAME_SIZE * pWingL->frame, FRAME_SIZE, FRAME_SIZE,
			RGB(0, 0, 0)
		);

		//// 绘制Boss到缓存
		//if (ProtectBoss)
		//	graph.DrawImage(png_Protect, pBoss->x - bgX[bgN] - 64, pBoss->y - bgY[bgN] - 64, 128, 128);
		//SelectObject(hdc_loadBmp, date[2].img);
		//TransparentBlt
		//(
		//	hdc_memBuffer, pBoss->x - bgX[bgN] - (FRAME_SIZE >> 1), pBoss->y - bgY[bgN] - (FRAME_SIZE >> 1),
		//	FRAME_SIZE, FRAME_SIZE,
		//	hdc_loadBmp, 0, FRAME_SIZE * pBoss->frame, FRAME_SIZE, FRAME_SIZE,
		//	RGB(0, 0, 0)
		//);


		//// 绘制敌人到缓存
		//for (i = 1; i <= nEnemy; i++)
		//{
		//	if (pEnemy[i])
		//	{
		//		SelectObject(hdc_loadBmp, date[pEnemy[i]->ID].img);
		//		TransparentBlt
		//		(
		//			hdc_memBuffer, pEnemy[i]->x - bgX[bgN] - (FRAME_SIZE >> 1), pEnemy[i]->y - bgY[bgN] - (FRAME_SIZE >> 1),
		//			FRAME_SIZE, FRAME_SIZE,
		//			hdc_loadBmp, 0, FRAME_SIZE * pEnemy[i]->frame, FRAME_SIZE, FRAME_SIZE,
		//			RGB(0, 0, 0)
		//		);
		//	}
		//}
		//// 绘制子弹到缓存
		//for (i = 1; i <= nHBullet; i++)
		//{
		//	SelectObject(hdc_loadBmp, date[pHBullet[i]->ID].img);
		//	TransparentBlt
		//	(
		//		hdc_memBuffer, pHBullet[i]->x - bgX[bgN] - date[pHBullet[i]->ID].radius, pHBullet[i]->y - bgY[bgN] - date[pHBullet[i]->ID].radius,
		//		date[pHBullet[i]->ID].radius << 1, date[pHBullet[i]->ID].radius << 1,
		//		hdc_loadBmp, 0, 0, BULLET_SIZE, BULLET_SIZE,
		//		RGB(0, 0, 0)
		//	);
		//}
		//for (i = 1; i <= nEBullet; i++)
		//{
		//	SelectObject(hdc_loadBmp, date[pEBullet[i]->ID].img);
		//	TransparentBlt
		//	(
		//		hdc_memBuffer, pEBullet[i]->x - bgX[bgN] - date[pEBullet[i]->ID].radius, pEBullet[i]->y - bgY[bgN] - date[pEBullet[i]->ID].radius,
		//		date[pEBullet[i]->ID].radius << 1, date[pEBullet[i]->ID].radius << 1,
		//		hdc_loadBmp, 0, 0, BULLET_SIZE, BULLET_SIZE,
		//		RGB(0, 0, 0)
		//	);
		//}

		// 绘制爆炸动画到缓存
		for (i = 1; i <= nAnimation; i++)
		{
			graph.DrawImage(png_Crash, pAnimation[i]->x - bgX[bgN] - date[pAnimation[i]->ID].radius, pAnimation[i]->y - bgY[bgN] - date[pAnimation[i]->ID].radius,
				((pAnimation[i]->HP / ANIMATION_FRAME) % 4) * 64, (((pAnimation[i]->HP / ANIMATION_FRAME) / 4)) * 64, 64, 64, UnitPixel);
		}


		//// 绘制主角状态
		//switch (pHero->HP)
		//{
		//	case 5:
		//		graph.DrawImage(png_HPHero[1], 50, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 100, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 150, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 200, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 250, WINDOW_HEIGHT - 150, 36, 36);
		//		break;
		//	case 4:
		//		graph.DrawImage(png_HPHero[1], 50, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 100, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 150, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 200, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 250, WINDOW_HEIGHT - 150, 36, 36);
		//		break;
		//	case 3:
		//		graph.DrawImage(png_HPHero[1], 50, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 100, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 150, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 200, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 250, WINDOW_HEIGHT - 150, 36, 36);
		//		break;
		//	case 2:
		//		graph.DrawImage(png_HPHero[1], 50, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[1], 100, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 150, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 200, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 250, WINDOW_HEIGHT - 150, 36, 36);
		//		break;
		//	case 1:
		//		graph.DrawImage(png_HPHero[1], 50, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 100, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 150, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 200, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 250, WINDOW_HEIGHT - 150, 36, 36);
		//		break;
		//	default:
		//		graph.DrawImage(png_HPHero[0], 50, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 100, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 150, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 200, WINDOW_HEIGHT - 150, 36, 36);
		//		graph.DrawImage(png_HPHero[0], 250, WINDOW_HEIGHT - 150, 36, 36);
		//		
		//}
		//switch (MPHero)
		//{
		//case 5:
		//	graph.DrawImage(png_MPHero[1], 50, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 100, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 150, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 200, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 250, WINDOW_HEIGHT - 100, 36, 36);
		//	break;
		//case 4:
		//	graph.DrawImage(png_MPHero[1], 50, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 100, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 150, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 200, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 250, WINDOW_HEIGHT - 100, 36, 36);
		//	break;
		//case 3:
		//	graph.DrawImage(png_MPHero[1], 50, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 100, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 150, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 200, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 250, WINDOW_HEIGHT - 100, 36, 36);
		//	break;
		//case 2:
		//	graph.DrawImage(png_MPHero[1], 50, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[1], 100, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 150, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 200, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 250, WINDOW_HEIGHT - 100, 36, 36);
		//	break;
		//case 1:
		//	graph.DrawImage(png_MPHero[1], 50, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 100, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 150, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 200, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 250, WINDOW_HEIGHT - 100, 36, 36);
		//	break;
		//default:
		//	graph.DrawImage(png_MPHero[0], 50, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 100, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 150, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 200, WINDOW_HEIGHT - 100, 36, 36);
		//	graph.DrawImage(png_MPHero[0], 250, WINDOW_HEIGHT - 100, 36, 36);
		//}
		////绘制Boss状态
		//i = pBoss->HP; i -= 1; i /= 10; i += 1;
		//if ((i >= 1) && (i <= 8))
		//	graph.DrawImage(png_HPBoss[i], pBoss->x - bgX[bgN] - 88, pBoss->y - bgY[bgN] - 80, 176, 36);
		//

	}
	else if ((currentStage >= 0) && (currentStage <= 10))
	{
		SelectObject(hdc_loadBmp, bmp_bg[currentStage]);
		StretchBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadBmp, 0, 0, BGX_SIZE, BGY_SIZE, SRCCOPY);
	}


	// 最后将所有的信息绘制到屏幕上
	BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);
	// 回收资源所占的内存（非常重要）
	DeleteObject(blankBmp);
	DeleteDC(hdc_memBuffer);
	DeleteDC(hdc_loadBmp);

	// 结束绘制
	EndPaint(hWnd, &ps);

}
#pragma endregion
