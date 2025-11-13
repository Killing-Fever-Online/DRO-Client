#ifndef LEGACYBACKGROUNDREADER_H
#define LEGACYBACKGROUNDREADER_H
#include "background_data.h"


class LegacyBackgroundReader : public BackgroundData
{
public:
  LegacyBackgroundReader();

  void execLoadBackground(QString t_backgroundName);

private:
  void processBackgroundNoIni(QString t_backgroundName);
};

#endif // LEGACYBACKGROUNDREADER_H
