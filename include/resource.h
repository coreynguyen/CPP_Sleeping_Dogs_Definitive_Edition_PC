#ifndef APPRESOURCE
    #define APPRESOURCE

   // #define ID_EN_US_AFF                   1001
   // #define ID_EN_US_DIC                   1002
    #define MAINICON                    ICON                     "icon.ico"
   // #define IDI_ICON                    101

    #define VER_FILEVERSION             3,10,349,0
    #define VER_FILEVERSION_STR         "3.10.349.0\0"
    #define VER_PRODUCTVERSION          3,10,0,0
    #define VER_PRODUCTVERSION_STR      "3.10\0"

    #ifndef DEBUG
        #define VER_DEBUG              0
    #else
        #define VER_DEBUG              VS_FF_DEBUG
    #endif

    #ifndef IDC_STATIC
        #define IDC_STATIC (-1)
    #endif
#endif // header guard

