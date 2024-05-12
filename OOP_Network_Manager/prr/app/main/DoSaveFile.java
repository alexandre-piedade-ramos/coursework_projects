package prr.app.main;

import prr.core.NetworkManager;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import prr.core.exception.MissingFileAssociationException;
/**
 * Command to save a file.
 */
class DoSaveFile extends Command<NetworkManager> {

  DoSaveFile(NetworkManager receiver) {
    super(Label.SAVE_FILE, receiver);
  }
  
  @Override
  protected final void execute()  {
      
      try {
        _receiver.save();
      }
      catch(MissingFileAssociationException exc){
        saveAs();
      }
      catch (Exception exc) {
        exc.printStackTrace(); //not sure
      } 
    }

  private void saveAs(){
    try {
      _receiver.saveAs(Form.requestString(Message.newSaveAs()));
    }
    catch (Exception exc) {
      exc.printStackTrace(); //not sure
    }
  }

}

