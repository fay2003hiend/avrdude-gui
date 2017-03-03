#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <QComboBox>

namespace Ui {
class Widget;
}

class ZProcess : public QProcess
{
    Q_OBJECT
public:
    QByteArray std_err_data;
    QByteArray std_out_data;

    ZProcess(QObject *parent = 0);

    int execute(const QString &command);
private slots:
    void slot_read_stdout();
    void slot_read_stderr();
};

class Widget : public QWidget
{
    Q_OBJECT

    QString last_programmer;
    QString last_device;
    QString last_arguments;
    QString last_hfuse;
    QString last_lfuse;
    QString last_efuse;
    QString last_path;

    bool do_fuse;
    bool has_hfuse;
    bool has_efuse;

    bool fill_combobox(const QString& prog, QComboBox *box, const QString &matching_d);

    void fromUI();
    void toUI();

    void do_cmd(const QString& path);
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *event);

private slots:
    void slot_init();
    void slot_cmd();
    void slot_read();
    void slot_calc();

    void slot_do_fuse(bool);
    void slot_hfuse(bool);
    void slot_efuse(bool);
private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
