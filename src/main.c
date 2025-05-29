#include <stdio.h>

#include "app/app.h"

int main(void) {
    struct App app;
    enum AppErr err = APP_ERR_OK;
    if ((err = App_initialize(&app, "data/words.txt"))) {
        printf("%d\n", err);
        return 1;
    }
    if ((err = App_log(&app, stdout))) {
        printf("%d\n", err);
        return 2;
    }
    App_finalize(&app);
    return 0;
}
