package prr.app.terminal;

import prr.core.exception.SameStateException;
import prr.core.Network;
import prr.core.terminal.*;
import pt.tecnico.uilib.menus.CommandException;

/**
 * Turn on the terminal.
 */
class DoTurnOnTerminal extends TerminalCommand {

  DoTurnOnTerminal(Network context, Terminal terminal) {
    super(Label.POWER_ON, context, terminal);
  }
  
  @Override
  protected final void execute() throws CommandException {
    try{
      _receiver.toIdle();
    }
    catch(SameStateException exc){
      _display.popup(Message.alreadyOn());
    }

  }
}
