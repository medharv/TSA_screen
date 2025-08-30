#include <QApplication>
#include "diagramwidget.h"

/**
 * @brief Main entry point for TSA Screen application
 * 
 * Creates the Qt application and main TSA display widget.
 * The TSAWidget handles all simulation and rendering logic.
 * 
 * @param argc Command line argument count
 * @param argv Command line arguments array
 * @return Application exit code
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Create and show the main TSA display widget
    TSAWidget widget;
    widget.show();
    
    return app.exec();
} 