#include "VulkanWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("windows");

    VulkanWidget w;
    w.show();
    return QApplication::exec();
}
