#include <windows.h>

int main()
{
    if (MessageBox(NULL,"Warning!!!!!\nThis software is a Trojan. It will make this computer unusable.\nIf you are serious, you can press \"yes\" to continue.\nOther close it quickly!!!!!\nARE YOU GOING TO EXECUTE IT???","Trojan Alert!!!",MB_YESNO | MB_ICONWARNING) != IDYES || MessageBox(NULL,"THIS IS THE LAST WARNING!!!!!\n\t\nSTILL EXECUTE IT???","Trojan Alert!!!",MB_YESNO | MB_ICONWARNING) != IDYES)
    {
        return 1;
    }
    return 0;
}