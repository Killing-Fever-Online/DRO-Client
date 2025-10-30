#include "screenshot_button.h"
#include "modules/theme/thememanager.h"
#include "dro/interface/courtroom_layout.h"
#include "dro/system/audio.h"

ScreenshotButton::ScreenshotButton(QWidget *parent, AOApplication *p_ao_app) : RPButton(parent)
{
  ao_app = p_ao_app;
  connect(this, SIGNAL(clicked()), this, SLOT(OnButtonClicked()));
  UpdateDimensions();
  show();
}

void ScreenshotButton::UpdateDimensions()
{
  ThemeManager::get().AssignDimensions(this, "screenshot", RPSceneType::COURTROOM);
  set_theme_image("screenshot", "screenshot.png", "courtroom", "Screenshot");
}

void ScreenshotButton::OnButtonClicked()
{
  audio::system::Play("screenshot");

  QString outputFilename = QDateTime::currentDateTimeUtc().toString(ao_app->log_timestamp);
  QString outputPath = "screenshots/" + outputFilename + ".png";
  ao_app->ensure_directory(outputPath);

  courtroom::viewport::screenshot(outputPath);
}
