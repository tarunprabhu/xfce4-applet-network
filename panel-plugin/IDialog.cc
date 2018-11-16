#include "IDialog.h"
#include "Utils.h"

Response IDialog::runDialog() {
  GtkWidget* dialog = createDialog();

  // Blocking call
  int response = gtk_dialog_run(GTK_DIALOG(dialog));
  
  destroyDialog();
  clearDialog();

  return convertResponse(response);
}
