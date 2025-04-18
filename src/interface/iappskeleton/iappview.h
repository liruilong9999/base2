#ifndef IAPPVIEW_H__20241109
#define IAPPVIEW_H__20241109

class QMainWindow;
class QString;
class QAction;
class QMenuBar;

class IAppView
{
public:
    // 注册组
    virtual void registerGroup(const QString & groupName) = 0;

    // 注册页
    virtual void registerPage(const QString & groupName, const QString & pageName) = 0;

    // 注册动作
    virtual void registerAction(const QString & groupName, const QString & pageName, QAction * action) = 0;

    // 获取主窗口
    virtual QMainWindow * getMainWindow() = 0;

    // 创建菜单栏
    virtual void createMenuBar(QMenuBar * menuBar) = 0;
};

#endif // !IAPPVIEW_H__
