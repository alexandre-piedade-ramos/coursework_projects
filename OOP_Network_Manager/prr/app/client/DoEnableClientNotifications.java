package prr.app.client;

import prr.core.Network;
import prr.app.exception.UnknownClientKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import prr.core.exception.UnknownClientKeyExceptionCore;

/**
 * Enable client notifications.
 */
class DoEnableClientNotifications extends Command<Network> {

  DoEnableClientNotifications(Network receiver) {
    super(Label.ENABLE_CLIENT_NOTIFICATIONS, receiver);
    addStringField("key", Message.key());
  }
  
  @Override
  protected final void execute() throws CommandException {
    String key = stringField("key");
    try{
      if(!_receiver.enableClientNotifs(key)){
        _display.popup(Message.clientNotificationsAlreadyEnabled());
      }
    }
    catch(UnknownClientKeyExceptionCore exc){
      throw new UnknownClientKeyException(exc.getKey());
    }
  }
}
