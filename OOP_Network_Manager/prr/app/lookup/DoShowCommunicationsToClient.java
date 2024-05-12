package prr.app.lookup;

import prr.core.Network;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import prr.core.exception.UnknownClientKeyExceptionCore;
import prr.app.exception.UnknownClientKeyException;

/**
 * Show communications to a client.
 */
class DoShowCommunicationsToClient extends Command<Network> {

  DoShowCommunicationsToClient(Network receiver) {
    super(Label.SHOW_COMMUNICATIONS_TO_CLIENT, receiver);
    addStringField("key", Message.clientKey());
  }

  @Override
  protected final void execute() throws CommandException {
    String key = stringField("key");
    try{
      _display.popup(_receiver.getClientReceivedCommsAsStrings(key));
    }catch(UnknownClientKeyExceptionCore exc){
      throw new UnknownClientKeyException(exc.getKey());
    }
  }
}
