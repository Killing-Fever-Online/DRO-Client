#include "eventfilters.h"

bool RPLineEditFilter::eventFilter(QObject *obj, QEvent *event)
{
  QLineEdit *line_edit = qobject_cast<QLineEdit *>(obj);
  if (line_edit != nullptr) {
    // Key press detection
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);

      if (keyEvent != nullptr) {
        if ((keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) &&
            (keyEvent->modifiers() & Qt::ShiftModifier)) {
          QString all_text = line_edit->text();
          int selection_start = line_edit->selectionStart();
          int selection_length = line_edit->selectionLength();
          all_text.remove(selection_start, selection_length);
          // Due to this being a LineEdit, aka no multilines, insert it as a "\n" instead of an actual newline
          all_text.append("\\n");
          line_edit->setText(all_text);
          return true;
        } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
          if (rate_limited) {
            return false;
          }
          emit chat_return_pressed();
          return true;
        }
      }
    }

    // Focus-out event handling
    if (event->type() == QEvent::FocusOut && text_edit_preserve_selection) {
      // We check if there's a selection
      if (line_edit->hasSelectedText()) {
        int selection_start = line_edit->selectionStart();
        int selection_length = line_edit->selectionLength();
        if (selection_start != -1 && selection_length != -1) {
          line_edit->setSelection(selection_start, selection_length);
          return true;
        }
      }
      else
      {
        // We don't have a selection. Remove focus.
        line_edit->clearFocus();
      }
    }
  }
  return false;
}
