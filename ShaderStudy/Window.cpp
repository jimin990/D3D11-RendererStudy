#include "Window.h"
#include <windows.h>

/*
* LRESULT: 메시지 처리 결과를 반환하는 자료형
* CALLBACK: Windows가 요구하는 함수 호출 규약
* 
*  HWND hwnd: 메시지가 발생한 창의 핸들
*  UINT message: 어떤 메시지인지 나타내는 번호 
*  WPARAM wParam: 메시지에 딸린 추가 정보
*  LPARAM lParam: 메시지에 딸린 추가 정보
*/
LRESULT CALLBACK Window::WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message) 
    {
    case WM_DESTROY:

        // 창이 파괴되면 메시지 루프에 종료를 알림
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_UP)
        {
            //transformData.scale += 0.1f;
        }
        else if (wParam == VK_DOWN)
        {
            //transformData.scale -= 0.1f;
        }
        return 0;
    }

    /*
    * 따로 지정해주지 않은 메시지를 기본 처리 함수에 맡기는 함수
    * 이로서 창 이동, 크기 변경, 닫기 버튼 같은 기본 동작을 하나하나 직접 구현하지 않아도 된다.
    */
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool Window::Create(
    HINSTANCE instance,
    int width,
    int height,
    const wchar_t* title
)
{
    /*
    * 현재 프로세스의 실행 파일(EXE) 모듈 핸들을 가르킨다.
    * EXE 모듈이란, 현재 실행 중인 EXE의 코드와 데이터라고 생각하면 된다.
    * 프로세스랑 다른 점은 프로세스는 프로그램 전체, 모듈은 그 안에 로드된 구성 요소이다.
    * wWinMain으로 실행하게 되면 인자로 들어오기 때문에 생략한다.
    */
    //HINSTANCE instance = GetModuleHandleW(nullptr);

    /*
    * 창의 기본 설정을 담는 구조체{}는 모두 0으로 초기화 한다는것이다.
    * 여기서 클래스는 c++ 에서 말하는 클래스를 말하는 것이 아니다.
    * 어떤 설정으로 창을 만들지 정해 놓은 틀을 말한다.
    */
    WNDCLASSW wc{};

    /*
    * 창의 메시지를 처리할 함수의 주소를 저장하는 멤버. 즉, 함수 포인터
    * 이 종류의 창에서 메시지가 발생하면 설정한 함수를 호출한다.
    * 이때 함수 호출을 위한 주소를 저장하기 때문에 ()는 붙히지 않는다.
    */
    wc.lpfnWndProc = WindowProc;

    /*
    * 이 창이 속한 모듈을 설정한다.
    * 앞서 만든 모듈을 지정한다.
    */
    wc.hInstance = instance;

    /*
    * 추후 창을 만들때 사용할 이 설정의 이름을 지정한다.
    * 창 제목이랑 다른 개념이다.
    * L 이란 Window의 w버전 함수에서 사용하는 와이드 문자열이라는 표시이다.
    */
    wc.lpszClassName = L"MyWindowClass";

    /*
    * 이 창에서 사용할 마우스 커서를 지정한다.
    * hCursor: 커서 핸들을 저장하는 멤버
    * LoadCursorW: 커서 리소스를 불러오는 함수
    * nullptr을 지정하면 윈도우의 기본 커서를 사용한다.
    * MAKEINTRESOUTCEW: 숫자 리소스 ID를 API에 전달할 수 있는 형태로 바꾼다.
    * 32512: 기본 화살표 커서의 리소스 번호
    */
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));

    /*
    * 창 내부의 기본 배경을 지정한다.
    * hbrBackcround: 배경을 칠할 브러시의 핸들
    * GetSysColorBrush: Windows 시스템 색상에 해당하는 브러시를 가져온다.
    * COLOR_WINDOW: 창 내부 배경에 사용하는 시스템 색상
    */
    wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);

    /*
    * 작성한 설정을 Windows에 등록한다.
    */
    RegisterClassW(&wc);

    // 2. 실제 창 생성
    hwnd = CreateWindowExW(
        0,                      // 추가 스타일
        L"MyWindowClass",       // 등록한 창 클래스 이름
        title,          // 제목
        WS_OVERLAPPEDWINDOW,    // 일반적인 데스크톱 창 스타일
        CW_USEDEFAULT,          // 시작 X 위치
        CW_USEDEFAULT,          // 시작 Y 위치
        width,                    // 창 전체 너비
        height,                    // 창 전체 높이
        nullptr,                // 부모 창 없음
        nullptr,                // 메뉴 없음
        instance,               // 프로그램 모듈 핸들
        nullptr                 // 추가 전달 데이터 없음
    );

    if (!hwnd)
    {
        // 1은 오류를 나태나는 값
        return false;
    }

    /*
    * SW_SHOW: 창을 표시하라는 명령
    * 숨기기, 최소화, 최대화 등 여러 명령을 할 수 있다.
    */
    ShowWindow(hwnd, SW_SHOW);

    return true;
}


/*
* 현재 스레드의 메시지 큐에서 메시지를 가져온다.
* 메시지는 총 3가지 종류로
* 양수: 일반 메시지
* 0: 종료 메시지 WM_QUIT
* -1: 오류 발생
* GetMessage는 메시지가 올때 까지 계속 기다리는 상태이다.

BOOL result = GetMessageW(&msg, nullptr, 0, 0);

if (result == -1) // 오류
{
    return -1;
}

if (result == 0) // WM_QUIT 수신
{
    break;
}
*/
bool Window::ProcessMessages()
{
    /*
    * 메시지 정보를 보관할 구조체
    */
    MSG msg{};

    /*
    * 메시지가 있으면 가져와서 true 반환
    * 메시지가 없으면 기다리지 않고 false 반환
    */
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }

        /*
        * 메시지값을 실제 문자 메시지 값으로 처리 될 수 있도록 변환한다.
        * 원래의 메시지는 어떤 키를 눌렸다는 처리만 가능하지만, 변환을 통해서
        * 어떤 키를 입력받았는지 알 수 있다.
        * 예를 들어 Shift + a 라면 A키가, 그냥 a 하면 a 키로 입력 받도록 할 수 있는 것이다.
        *
        */
        TranslateMessage(&msg);

        /*
        * 가져온 창 메시지를 해당 창의 처리 함수에 전달
        * 아까 설정한 wc.lpfnWndProc = WindowProc; 이곳에 설정
        */
        DispatchMessage(&msg);
    }

    return true;
}