#include "exception.h"
#include "main.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    UpdateProcessList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

struct Processes
{
    struct Info
    {
        DWORD pid;
        TCHAR name[MAX_PATH];
    };

    std::vector<Info> processes;

    void Enum()
    {
        processes.clear();

        DWORD processes[1024];
        DWORD bytes = 0;

        if(!::EnumProcesses(processes, sizeof(processes), &bytes))
            throw Exception() << "Can't find processes.";

        DWORD Nrocesses = bytes/sizeof(DWORD);

        for(int i = 0; i < Nrocesses; i++ )
            if(processes[i] != 0 )
                GetFileName(processes[i]);
    }

    void GetFileName(DWORD pid)
    {
        Info info = {pid, TEXT("<unknown>")};

        // Get a handle to the process.
        HANDLE process = ::OpenProcess( PROCESS_QUERY_INFORMATION |
                                       PROCESS_VM_READ,
                                       FALSE, pid);

        if(process)
        {
            HMODULE module;
            DWORD bytes;

            if(::EnumProcessModules( process, &module, sizeof(module), &bytes))
            {
                ::GetModuleBaseName(process, module, info.name,
                                   sizeof(info.name)/sizeof(TCHAR));
            }
        }

        ::CloseHandle(process);

        processes.push_back(info);
    }
};//struct Processes

void MainWindow::UpdateProcessList()
{
    Processes processes;
    processes.Enum();
}

