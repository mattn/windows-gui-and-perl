#include <windows.h>
#include "resource.h"
#include <perl.h>
#include "XSUB.h"

static PerlInterpreter *my_perl;
static int my_argc = 2;
static char *my_argv[] = { "perl", "-e:0", NULL };
static char **my_env;
static HWND hEdit = NULL;

extern void boot_DynaLoader (pTHX_ CV* cv);

void
XS_add_text(pTHX_ CV* cv) {
  dXSARGS;
  STRLEN n_a;
  char* text = (char*) SvPV(ST(0), n_a);
  int l = GetWindowTextLength(hEdit); 
  SendMessage(hEdit, EM_SETSEL, (WPARAM) l, (LPARAM) l);
  SendMessage(hEdit, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) text);
}

void
xs_init(pTHX) {
  newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, __FILE__);
}

void
hungly() {
  if (!my_perl) {
    PERL_SYS_INIT3(&my_argc, &my_argv, &my_env);
    my_perl = perl_alloc();
    PL_perl_destruct_level = 1;
    perl_construct(my_perl);
  }
  char path[MAX_PATH];
  char command[1024];

  GetModuleFileName(NULL, path, sizeof(path));
  strcpy(path + strlen(path) - 4, ".pl");

  PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
  perl_parse(my_perl, xs_init, my_argc, my_argv, my_env);
  perl_run(my_perl);

  newXSproto("add_text", XS_add_text, __FILE__, "$");

  snprintf(command, sizeof(command), "do q\"%s\"", path);
  eval_pv(command, TRUE);
}

LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
  HINSTANCE hInst;
  CREATESTRUCT *lpcs;

  switch (msg) {
    case WM_CREATE:
      lpcs = (CREATESTRUCT *)lp;
      hInst = lpcs->hInstance;
      hEdit = CreateWindow("EDIT", "",
          WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN | 
          ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_HSCROLL | WS_VSCROLL,
          0, 0, 0, 0, hWnd, (HMENU) ID_EDIT, hInst, NULL);
      SetFocus(hEdit);
      break;
    case WM_SIZE:
      MoveWindow(hEdit, 0, 0, LOWORD(lp), HIWORD(lp), TRUE);
      break;
    case WM_COMMAND:
      switch (LOWORD(wp)) {
        case IDM_EXIT:
          SendMessage(hWnd, WM_CLOSE, 0, 0);
          break;
        case IDM_HUNGRY:
          hungly();
          break;
      }
      break;
    case WM_CLOSE:
      DestroyWindow(hEdit);
      DestroyWindow(hWnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return (DefWindowProc(hWnd, msg, wp, lp));
  }
  return 0;
}


int WINAPI
WinMain(
    HINSTANCE hCurInst, HINSTANCE hPrevInst,
    LPSTR lpsCmdLine, int nCmdShow) {
  HWND hWnd;
  MSG msg;
  WNDCLASSEX wc;

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hCurInst;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = MAKEINTRESOURCE(ID_MENU);
  wc.lpszClassName = "JPerlAdventCalendar";
  wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  RegisterClassEx(&wc);

  hWnd = CreateWindow(wc.lpszClassName,
      "Perl Plugin Example",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      NULL,
      NULL);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}
