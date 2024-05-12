package prr.app.client;

import prr.core.Network;
import prr.app.exception.UnknownClientKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

import prr.core.exception.UnknownClientKeyExceptionCore;

/**
 * Show specific client: also show previous notifications.
 */
class DoShowClient extends Command<Network> {

  DoShowClient(Network receiver) {
    super(Label.SHOW_CLIENT, receiver);
    //FIXME add command fields
    addStringField("key", Message.key());
  }
  
  @Override
  protected final void execute() throws CommandException {

    String key = stringField("key");


    try{
      _display.popup(_receiver.clientToString(key));
      _display.popup(_receiver.clientNotifsAsStrings(key));
    }
    catch(UnknownClientKeyExceptionCore exc){
      throw new UnknownClientKeyException(key);
    }
  }
}
