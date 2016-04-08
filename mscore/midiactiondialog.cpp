#include "midiactiondialog.h"
#include <QFont>
namespace Ms {

/* TODO:
 * check noData and noData2 for MIDI event type: in progress
 * SYSEX combined ID
 * SYSEX adding new comboboxes
 * Test instrument channel: Works
 * Reimplement drop event: Works
 * Excerpts: Doesn't work by design
 * SystemText : Works
 * Writing all MIDI messages: Works
 * Problem with remapping actions: Fixed
 * Separate commits
 * Inspector (?)
 * Add mtests
 * record an example
 * add more standart events
 * Add more hints in Add New Event dialog
 * */


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
            {ME_NOTEOFF, " Note off"}, {ME_NOTEON," Note on"}, {ME_POLYAFTER, " Polyafter"}, {ME_CONTROLLER, " Controller"},
            {ME_PROGRAM, " Program change"}, {ME_AFTERTOUCH, " Aftertouch"}, {ME_PITCHBEND, " Pitch bend"}, {ME_SYSEX, " SysEx"},
            {ME_META, " Reset"}, {ME_MTC_QF, " MTC Quarter Frame"}, {ME_SONGPOS, " Songpos"}, {ME_SONG_SEL, " Song select"},
            {ME_TUNE_REQUEST, " Tune request"}, {ME_ENDSYSEX, " End SysEx"}, {ME_CLOCK, " Clock"}, {ME_START, " Start"},
            {ME_CONTINUE, " Continue"}, {ME_STOP, " Stop"}, {ME_SENSE, " Sense"}
      };

      for (int i = 0; i < 256; i++) {
            QString s = QString::number(i);
            if (i < 128) {
                  ui->dataA->addItem(s);
                  ui->dataB->addItem(s);
                  }

            int channelIndex = 0;
            if (i > ME_NOTEOFF && i < ME_NOTEON)
                  channelIndex = i - ME_NOTEOFF;
            else if (i > ME_NOTEON && i < ME_POLYAFTER)
                  channelIndex = i - ME_NOTEON;
            else if (i > ME_POLYAFTER && i < ME_CONTROLLER)
                  channelIndex = i - ME_POLYAFTER;
            else if (i > ME_CONTROLLER && i < ME_PROGRAM)
                  channelIndex = i - ME_CONTROLLER;
            else if (i > ME_PROGRAM && i < ME_AFTERTOUCH)
                  channelIndex = i - ME_PROGRAM;
            else if (i > ME_AFTERTOUCH && i < ME_PITCHBEND)
                  channelIndex = i - ME_AFTERTOUCH;
            else if (i > ME_PITCHBEND && i < ME_SYSEX)
                  channelIndex = i - ME_PITCHBEND;

            s += m.value(i - channelIndex);
            bool undefined = (i == 0xf4 || i == 0xf5 || i == 0xf9 || i == 0xfd); // undefined events
            bool grayed = channelIndex != 0 || undefined;
            if (channelIndex != 0)
                  ui->eventType->addItem(s + " (Ch "+QString::number(channelIndex + 1) + ")");
            else if (undefined)
                  ui->eventType->addItem(s + tr(" Undefined"));
            else
                  ui->eventType->addItem(s);

            if (grayed)
                  ui->eventType->setItemData(i, QColor(Qt::gray), Qt::TextColorRole);

           /* if ((i > ME_NOTEOFF && i < ME_SYSEX)
                && i != ME_NOTEON && i != ME_POLYAFTER && i != ME_AFTERTOUCH && i != ME_PITCHBEND && i != ME_CONTROLLER && i != ME_PROGRAM) {
                  ui->eventType->setItemData(i, QColor(Qt::gray), Qt::TextColorRole);
                  ui->eventType->addItem(s + " (Ch"+);
                  }
            else
                  ui->eventType->addItem(s);*/
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
      bool noData = (index >= ME_TUNE_REQUEST && index <= ME_META && index != 0xfd && index != 0xf9); // Tune request, End of SysEx and System real-time except two Undefined events
      bool noData2 = (index == ME_SONG_SEL ||
                     (index >= ME_PROGRAM && index <= ME_PITCHBEND));

      ui->dataA->setVisible(!noData);
      ui->dataAlabel->setVisible(!noData);
      ui->dataB->setVisible(!(noData || noData2));
      ui->dataBlabel->setVisible(!(noData || noData2));

      if (clearDataA == true) {
            ui->dataAlabel->setText(tr("Data A:"));
            ui->dataBlabel->setText(tr("Data B:"));
            ui->dataA->clear();
            for (int i = 0; i < 128; i++) {
                  QString s = QString::number(i);
                  ui->dataA->addItem(s);
                  //ui->dataA->it
                  }
            clearDataA = false;
            }

      QMap<int, QString> midiController = {
            {CTRL_HBANK, " Bank select MSB"}, {CTRL_LBANK, " Bank select LSB"}, {CTRL_HDATA, " Data Entry MSB"},
            {CTRL_HNRPN, " HNRPN"}, {CTRL_LNRPN, " LNRPN"}, {CTRL_HRPN, " HRPN"}, {CTRL_LRPN, " LRPN"},
            {CTRL_MODULATION, " Modulation"},{CTRL_BREATH, " Breath controller"}, {CTRL_FOOT, "  Foot controller"}, {CTRL_PORTAMENTO_TIME, " Portamento time"}, {CTRL_VOLUME, " Volume"},
            {CTRL_BALANCE, " Balance"}, {CTRL_PANPOT, " Pan"}, {CTRL_EXPRESSION, " Expression"}, {CTRL_EFFECT_1, " Effect controller 1"},
            {CTRL_EFFECT_2, " Effect controller 2"},
            {CTRL_GENERAL_PURPOSE_1, " General Purpose 1"}, {CTRL_GENERAL_PURPOSE_2, " General Purpose 2"}, {CTRL_GENERAL_PURPOSE_3, " General Purpose 3"},
            {CTRL_GENERAL_PURPOSE_4, " General Purpose 4"},
            {CTRL_LMODULATION, " Modulation LSB"}, {CTRL_LBREATH, " Breath ctrl LSB"}, {CTRL_LFOOT, "Foot ctrl LSB"},
            {CTRL_LPORTAMENTO_TIME, " Portamento time LSB"}, {CTRL_LDATA, " Data Entry LSB"}, {CTRL_LVOLUME, " Volume LSB"},
            {CTRL_LBALANCE, " Balance LSB"}, {CTRL_LPANPOT, " Pan LSB"}, {CTRL_LEXPRESSION, " Expression LSB"}, {CTRL_LEFFECT_1, " Effect ctrl 1 LSB"},
            {CTRL_LEFFECT_2, " Effect ctrl 2 LSB"}, {CTRL_LGENERAL_PURPOSE_1, " General Purpose 1 LSB"}, {CTRL_LGENERAL_PURPOSE_2, " General Purpose 2 LSB"},
            {CTRL_LGENERAL_PURPOSE_3, " General Purpose 3 LSB"}, {CTRL_LGENERAL_PURPOSE_4, " General Purpose 4 LSB"},
            {CTRL_GENERAL_PURPOSE_5, " General Purpose 5"}, {CTRL_GENERAL_PURPOSE_6, " General Purpose 6"}, {CTRL_GENERAL_PURPOSE_7, " General Purpose 7"},
            {CTRL_GENERAL_PURPOSE_8, " General Purpose 8"},
            {CTRL_SUSTAIN, " Sustain pedal switch"},
            {CTRL_PORTAMENTO, " Portamento switch"}, {CTRL_SOSTENUTO, " Sostenuto switch"}, {CTRL_SOFT_PEDAL, " Soft Pedal switch"},
            {CTRL_LEGATO_FOOTSWITCH, " Legato footswitch"}, {CTRL_HOLD_2, " Hold 2"}, {CTRL_SOUND_VARIATION, " Sound ctrl 1 (Sound variation)"},
            {CTRL_HARMONIC_CONTENT, " Sound ctrl 2 (Filter Resonance)"}, {CTRL_RELEASE_TIME, " Sound ctrl 3 (Release time)"}, {CTRL_ATTACK_TIME, " Sound ctrl 4 (Attack time)"},
            {CTRL_BRIGHTNESS, " Sound ctrl 5 (Brightness)"}, {CTRL_DECAY_TIME, " Sound ctrl 6 (Decay time)"}, {CTRL_VIBRATO_RATE, " Sound ctrl 7 (Vibrato rate)"},
            {CTRL_VIBRATO_DEPTH, " Spund ctrl 8 (Vibrato depth)"}, {CTRL_VIBRATO_DELAY, " Sound ctrl 9 (Vibrato delay)"},
            {CTRL_SOUND_UNDEFINED, " Sound ctrl 10 (Undefined)"},
            {CTRL_PORTAMENTO_CONTROL, " Portamento control"}, {CTRL_REVERB_SEND, " Effect 1 Depth (Reverb)"},
            {CTRL_TREMOLO, " Effect 2 Depth (Tremolo)"},
            {CTRL_CHORUS_SEND, " Effect 3 Depth (Chorus)"}, {CTRL_VARIATION_SEND, " Effect 4 Depth (Detune)"},
            {CTRL_PHASER, " Effect 5 Depth (Phaser)"},
            {CTRL_DATA_INCREMENT, " Data increment"}, {CTRL_DATA_DECREMENT, " Data decrement"},
            {CTRL_ALL_SOUNDS_OFF, " All sounds off"},
            {CTRL_RESET_ALL_CTRL, " Reset all controllers"}, {CTRL_LOCAL_OFF, " Local switch"}, {CTRL_ALL_NOTES_OFF, " All notes off"},
            {CTRL_OMNI_OFF, " Omni off"}, {CTRL_OMNI_ON, " Omni on"}, {CTRL_MONO_MODE, " Mono mode"}, {CTRL_POLY_MODE, " Poly mode"}

      };

      if (index >= ME_CONTROLLER && index < ME_PROGRAM ) {
            ui->dataAlabel->setText(tr("Controller:"));
            ui->dataBlabel->setText(tr("Value:"));
            ui->dataA->clear();
            for (int i = 0; i < 128; i++) {
                  QString s = QString::number(i);
                  s += midiController.value(i);
                  // Undefined controller events
                  bool grayed = (i == 0x03 || i == 0x09 || i == 0x0e || i == 0x0f || (i >= 0x14 && i <= 0x1f)
                              || i == 0x23 || i == 0x29 || i == 0x2e || i == 0x2f || (i >= 0x34 && i <= 0x3f)
                              || (i >= 0x55 && i <= 0x5a) || (i >= 0x66 && i <= 0x77) || index == CTRL_SOUND_UNDEFINED);
                  bool undefined = grayed && index != CTRL_SOUND_UNDEFINED;

                  if (undefined)
                        ui->dataA->addItem(s + tr(" Undefined"));
                  else
                        ui->dataA->addItem(s);

                  if (grayed)
                        ui->dataA->setItemData(i, QColor(Qt::gray), Qt::TextColorRole);
                  }
            clearDataA = true;
            }
      else if (index >= ME_NOTEON && index < ME_POLYAFTER) {
            ui->dataAlabel->setText(tr("Pitch:"));
            ui->dataBlabel->setText(tr("Volume:"));
            clearDataA = true;
            }
      else if (index >= ME_NOTEOFF && index < ME_NOTEON) {
            ui->dataAlabel->setText(tr("Pitch:"));
            ui->dataBlabel->setText(tr("Velocity:"));
            clearDataA = true;
            }
      else if (index == ME_SYSEX) {
            ui->dataAlabel->setText(tr("Manufacturer's ID:"));
            ui->dataBlabel->setText(tr("Device ID:"));
            ui->dataA->clear();
            ui->dataA->addItem("0 Combined ID");
            for (int i = 1; i <= 126; i++) {
                  QString s = QString::number(i);
                  ui->dataA->addItem(s);
                  }
            ui->dataA->addItem("127 All devices");
            clearDataA = true;
            }
      }
}
