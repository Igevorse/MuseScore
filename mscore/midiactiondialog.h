#ifndef MIDIACTIONDIALOG_H
#define MIDIACTIONDIALOG_H

#include <QDialog>
#include "libmscore/instrument.h"
#include "musescore.h"
#include "ui_midiactiondialog.h"

namespace Ms {

struct MidiActionItem;
class MidiActionDialog : public QDialog
      {
      Q_OBJECT
public:
      explicit MidiActionDialog(QWidget *parent = 0);
      ~MidiActionDialog();
      Ms::MidiActionItem ai;
public slots:
      void eventTypeChanged(int index);
      void accept();

private:
      Ui::MidiActionDialog *ui;
      bool clearDataA;
      };

}

#endif // MIDIACTIONDIALOG_H
