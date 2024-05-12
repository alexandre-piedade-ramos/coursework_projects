package prr.app.client;

import prr.core.Network;
import prr.app.exception.UnknownClientKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import prr.core.exception.UnknownClientKeyExceptionCore;
/**
 * Disable client notifications.
 */
class DoDisableClientNotifications extends Command<Network> {

  DoDisableClientNotifications(Network receiver) {
    super(Label.DISABLE_CLIENT_NOTIFICATIONS, receiver);
    addStringField("key", Message.key());
  }
  
  @Override
  protected final void execute() throws CommandException {
    String key = stringField("key");
    try{
      if(!_receiver.disableClientNotifs(key)){
        _display.popup(Message.clientNotificationsAlreadyDisabled());
      }
    }
    catch(UnknownClientKeyExceptionCore exc){
      throw new UnknownClientKeyException(exc.getKey());
    }
  }
}
