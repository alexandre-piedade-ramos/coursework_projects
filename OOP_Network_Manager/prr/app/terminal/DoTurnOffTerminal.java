package prr.app.terminal;

import prr.core.exception.SameStateException;
import prr.core.Network;
import prr.core.terminal.*;
import pt.tecnico.uilib.menus.CommandException;

/**
 * Turn off the terminal.
 */
class DoTurnOffTerminal extends TerminalCommand {

  DoTurnOffTerminal(Network context, Terminal terminal) {
    super(Label.POWER_OFF, context, terminal);
  }
  
  @Override
  protected final void execute() throws CommandException {
    try{
      _receiver.toOff();
    }
    catch(SameStateException exc){
      _display.popup(Message.alreadyOff());
    }
  }
}
