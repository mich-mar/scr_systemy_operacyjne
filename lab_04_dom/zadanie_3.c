
// 1) LINUX
// ps -ef | grep zadanie_2
// michal     28281   27942 99 12:14 pts/0    00:01:05 ./zadanie_2
//
// cat /proc/28281/status | grep Sig
// SigQ:   0/124791 - liczba sygnałów w kolejce oczekujących na obsługę (ktualnie/limit)
// SigPnd: 0000000000000000 - sygnały oczekujące na obsługę w kontekście procesu
// SigBlk: 0000000000000004 - sygnały zablokowane przez proces
// SigIgn: 0000000000002000 - sygnały ignorowane przez proces
// SigCgt: 0000000000004a00 - sygnały przechwycone przez proces, czyli sygnały, dla których zdefiniowano specjalne procedury obsługi

// 2) SOLARIS
// mmarkuze@diablo:~/Dokumenty/SCR_systemy_operacyjne/lab_04_dom$ pflags 17751
// 17751:  ./zadanie_2
//        data model = _ILP32  flags = MSACCT|MSFORK
// /1:    flags = 0
//        sigmask = 0x0001e000,0x00000000

// > (MSACCT) - ma włączone szczegółowe śledzenie zasobów (MSACCT)
// > (MSFORK) - proces jest gotowy lub zdolny do tworzenia nowych procesów
// > (_IPL32) określa model danych używany przez proces (rozmiary typów int, long, i wskaźników są 32-bitowe)
// > (sigmask) - wskazują na maskę sygnałów procesu – sygnały, które są aktualnie zablokowane w danym procesie


#include <stdio.h>
#include <signal.h> 
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

int sigterm = 0, sigusr1 = 0, sigusr2 = 0; // flagi do odbierania sygnałów
bool intervals = false; // bool do interwałów odbierania sygnałów

void handleSigterm()
{
    sigterm = 1;
}

void handleSigusr1()
{
    sigusr1 = 1;
}

void handleSigusr2()
{
    sigusr2 = 1;
}

void handleFlags()
{
    // wyświetl komunikat i zakończ pracę programu
    if (sigterm == 1)
    {
        printf("SIGTERM - zakończenie pracy programu\n");
        sigterm = 0;
        exit(0);
    }

    // wyświetlenie komunikatu i powrót do pracy
    if (sigusr1 == 1)
    {
        printf("SIGUSR1 - komunikat\n");
        sigusr1 = 0;
    }

    // wyświetlenie komunikatu i zmiana interwałów
    if (sigusr2 == 1 && intervals == true)
    {
        printf("SIGUSR2 - interwały wyłączone\n");
        intervals = false;
        sigusr2 = 0;
    }
    else if (sigusr2 == 1 && intervals == false)
    {
        printf("SIGUSR2 - interwały włączone\n");
        intervals = true;
        sigusr2 = 0;
    }
}

int main()
{
    int idx = 0;
    bool receive = true;
    sigset_t sigList;

    // ignorowanie sygnału SIGALRM i ustawienie obsługi dla innych sygnałów
    signal(SIGALRM, SIG_IGN);
    signal(SIGTERM, handleSigterm);
    signal(SIGUSR1, handleSigusr1);
    signal(SIGUSR2, handleSigusr2);

    // inicjalizacja zbioru sygnałów, które będziemy blokować i odblokowywać.
    sigemptyset(&sigList);
    sigaddset(&sigList, SIGALRM);
    sigaddset(&sigList, SIGUSR1);
    sigaddset(&sigList, SIGUSR2);
    sigaddset(&sigList, SIGTERM);

    while (1)
    {
        idx++;

        if (intervals == true)
        {
            if (idx % 1000 == 0)
                receive = !receive;

            if (receive == true)
            {
                // odblokowanie całego zbioru sygnałów
                sigprocmask(SIG_UNBLOCK, &sigList, NULL);
            }
            else
            {
                // blokowanie całego zbioru sygnałów
                sigprocmask(SIG_BLOCK, &sigList, NULL);
            }
        }
        else
        {
            // Jeśli nie w interwałach, odblokowujemy sygnały
            sigprocmask(SIG_UNBLOCK, &sigList, NULL);
        }

        handleFlags();
    }

    return 0;
}

