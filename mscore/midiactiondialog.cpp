#include "midiactiondialog.h"

namespace Ms {

//---------------------------------------------------------
//   MidiActionDialog
//---------------------------------------------------------

MidiActionDialog::MidiActionDialog(QWidget *parent) :
      QDialog(parent),
      ui(new Ui::MidiActionDialog)
      {
      ui->setupUi(this);
      this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
      QMap<int, QString> m = {
            {0x80, " Note off"}, {0x90," Note on"}, {0xa0, " Polyafter"}, {0xb0, " Controller"},
            {0xc0, " Program change"}, {0xd0, " Aftertouch"}, {0xe0, " Pitch bend"}, {0xf0, " SysEx"},
            {0xff, " Meta"}, {0xf2, " Songpos"}, {0xf7, " End SysEx"}, {0xf8, " Clock"},
            {0xfa, " Start"}, {0xfb, " Continue"}, {0xfc, " Stop"}, {0xfe, " Sense"}
      };
      for (int i = 0; i < 255; i++) {
            QString s = QString::number(i);
            if (i < 128) {
                  ui->dataA->addItem(s);
                  ui->dataB->addItem(s);
                  }
            s += m.value(i);
            ui->eventType->addItem(s);
            }
      clearDataA = false;
      connect(ui->eventType , SIGNAL(currentIndexChanged(int)),this,SLOT(eventTypeChanged(int)));
      }

//---------------------------------------------------------
//   ~MidiActionDialog
//---------------------------------------------------------

MidiActionDialog::~MidiActionDialog()
      {
      delete ui;
      }

//---------------------------------------------------------
//   accept
//---------------------------------------------------------

void MidiActionDialog::accept()
      {
      if (ui->eventName->text().isEmpty()) {
            QToolTip::showText(ui->eventName->mapToGlobal(QPoint()), tr("Name could not be empty!"));
            return;
            }

      ai.descr = ui->eventName->text();
      ai.event.setType(ui->eventType->currentIndex());
      ai.event.setDataA(ui->dataA->currentIndex());
      ai.event.setDataB(ui->dataB->currentIndex());

      // check if name or event already exist
      for (int i = 0; i < mscore->midiActionList()->size(); ++i) {
            MidiActionItem mai = mscore->midiActionList()->at(i);
            if (mai.descr == ai.descr) {
                  QToolTip::showText(ui->eventName->mapToGlobal(QPoint()), tr("Event with this name already exists!"));
                  return;
                  }
            if (mai.event.type() == ai.event.type() && mai.event.dataA() == ai.event.dataA()
                && mai.event.dataB() == ai.event.dataB()) {
                  QToolTip::showText(ui->buttonBox->mapToGlobal(QPoint()), tr("Event already exists with name: %1").arg(mai.descr));
                  return;
                  }
            }
      this->done(QDialog::Accepted);
      }

//---------------------------------------------------------
//   eventTypeChanged
//---------------------------------------------------------

void MidiActionDialog::eventTypeChanged(int index)
      {
      if (clearDataA == true) {
            ui->dataAlabel->setText(tr("Data A:"));
            ui->dataBlabel->setText(tr("Data B:"));
            ui->dataA->clear();
            for (int i = 0; i < 128; i++) {
                  QString s = QString::number(i);
                  ui->dataA->addItem(s);
                  }
            clearDataA = false;
            }

      QMap<int, QString> midiController = {
            {CTRL_HBANK, " Bank select MSB"}, {CTRL_LBANK, " Bank select LSB"}, {CTRL_HDATA, " Data Entry MSB"}, {CTRL_LDATA, " Data Entry LSB"},
            {CTRL_HNRPN, " HNRPN"}, {CTRL_LNRPN, " LNRPN"}, {CTRL_HRPN, " HRPN"}, {CTRL_LRPN, " LRPN"},
            {CTRL_MODULATION, " Modulation"}, {CTRL_PORTAMENTO_TIME, " Portamento time"}, {CTRL_VOLUME, " Volume"},
            {CTRL_PANPOT, " Pan"}, {CTRL_EXPRESSION, " Expression"}, {CTRL_SUSTAIN, " Sustain pedal switch"},
            {CTRL_PORTAMENTO, " Portamento switch"}, {CTRL_SOSTENUTO, " Sostenuto switch"}, {CTRL_SOFT_PEDAL, " Soft Pedal switch"},
            {CTRL_HARMONIC_CONTENT, " Filter Resonance"}, {CTRL_RELEASE_TIME, " Release time"}, {CTRL_ATTACK_TIME, " Attack time"},
            {CTRL_BRIGHTNESS, " Brightness"}, {CTRL_PORTAMENTO_CONTROL, " Portamento control"}, {CTRL_REVERB_SEND, " Reverb"},
            {CTRL_CHORUS_SEND, " Chorus"}, {CTRL_VARIATION_SEND, " Detune"}, {CTRL_ALL_SOUNDS_OFF, " All sounds off"},
            {CTRL_RESET_ALL_CTRL, " Reset all controllers"}, {CTRL_LOCAL_OFF, " Local switch"}, {CTRL_ALL_NOTES_OFF, " All notes off"}

      };

      if (index == ME_CONTROLLER) {
            ui->dataAlabel->setText(tr("Controller:"));
            ui->dataBlabel->setText(tr("Value:"));
            ui->dataA->clear();
            for (int i = 0; i < 128; i++) {
                  QString s = QString::number(i);
                  s += midiController.value(i);
                  ui->dataA->addItem(s);
                  }
            clearDataA = true;
            }
      else if (index == ME_NOTEON) {
            ui->dataAlabel->setText(tr("Pitch:"));
            ui->dataBlabel->setText(tr("Volume:"));
            clearDataA = true;
            }
      else if (index == ME_NOTEOFF) {
            ui->dataAlabel->setText(tr("Pitch:"));
            ui->dataBlabel->setText(tr("Velocity:"));
            clearDataA = true;
            }
      }
}
