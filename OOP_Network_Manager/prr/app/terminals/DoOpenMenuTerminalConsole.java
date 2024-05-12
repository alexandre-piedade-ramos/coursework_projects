package prr.app.terminals;

import prr.core.Network;
import prr.core.exception.*;
import prr.core.terminal.Terminal;
import prr.app.exception.UnknownTerminalKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

/**
 * Open a specific terminal's menu.
 */
class DoOpenMenuTerminalConsole extends Command<Network> {

  DoOpenMenuTerminalConsole(Network receiver) {
    super(Label.OPEN_MENU_TERMINAL, receiver);
    //FIXME add command fields
    addStringField("id", Message.terminalKey());
  }

  @Override
  protected final void execute() throws CommandException, 
                                        UnknownTerminalKeyException {

    // create an instance of prr.app.terminal.Menu with the
    // selected Terminal and open it

    String id = stringField("id");
    Terminal terminal;

    try{
      terminal=_receiver.fetchTerminal(id);
    }
    catch(UnknownTerminalKeyExceptionCore exc){
      throw new UnknownTerminalKeyException(exc.getKey());
    }

    (new prr.app.terminal.Menu(_receiver, terminal)).open();
  }
}
