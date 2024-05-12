package prr.app.lookup;

import prr.core.Network;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import prr.core.exception.UnknownClientKeyExceptionCore;
import prr.app.exception.UnknownClientKeyException;


/**
 * Show communications from a client.
 */
class DoShowCommunicationsFromClient extends Command<Network> {

  DoShowCommunicationsFromClient(Network receiver) {
    super(Label.SHOW_COMMUNICATIONS_FROM_CLIENT, receiver);
    addStringField("key", Message.clientKey());
  }

  @Override
  protected final void execute() throws CommandException {
    String key = stringField("key");
    try{
      _display.popup(_receiver.getClientSentCommsAsStrings(key));
    }catch(UnknownClientKeyExceptionCore exc){
      throw new UnknownClientKeyException(exc.getKey());
    }
  }
}
