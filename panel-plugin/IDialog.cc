#include "IDialog.h"

IDialog::IDialog() : dialog(nullptr) {
  ;
}

void IDialog::saveDialog(GtkWidget* dlg) {
  dialog = dlg;
}

void IDialog::destroyDialog() {
  if(dialog)
    gtk_widget_destroy(dialog);
  dialog = nullptr;
}

int IDialog::runDialog() {
  dialog = createDialog();

  // Blocking call
  int response = gtk_dialog_run(GTK_DIALOG(dialog));
  
  destroyDialog();
  clearDialog();

  return response;
}

GtkWidget* IDialog::getDialog() {
  return dialog;
}
