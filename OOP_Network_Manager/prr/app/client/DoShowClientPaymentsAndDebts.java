package prr.app.client;

import prr.core.Network;
import prr.app.exception.UnknownClientKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import prr.core.exception.UnknownClientKeyExceptionCore;
import prr.core.client.Client;

/**
 * Show the payments and debts of a client.
 */
class DoShowClientPaymentsAndDebts extends Command<Network> {

  DoShowClientPaymentsAndDebts(Network receiver) {
    super(Label.SHOW_CLIENT_BALANCE, receiver);
    addStringField("ket",Message.key());
  }
  
  @Override
  protected final void execute() throws CommandException {
    String key = stringField("key");
    try{
      Client c = _receiver.fetchClient(key);
      _display.popup(Message.clientPaymentsAndDebts(key, (long) c.getPayments(), (long) c.getDebts()));
    }
    catch(UnknownClientKeyExceptionCore exc){
      throw new UnknownClientKeyException(exc.getKey());
    }
  }
}
