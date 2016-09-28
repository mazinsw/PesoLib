package br.com.mzsw;

/**
 * Interface que recebe os eventos de recebimento de peso e conexão com uma balança
 * 
 * @author Mazin
 *
 */
public interface BalancaListener {
	/**
	 * Conectou-se a uma balança
	 * 
	 * @param sender instancia da balança
	 */
	public void onConectado(Object sender);
	
	/**
	 * Recebeu um peso da balança
	 * 
	 * @param sender instancia da balança
	 * @param gramas peso em gramas do item que está sobre a balança
	 */
	public void onPesoRecebido(Object sender, int gramas);
	
	/**
	 * Desconectou-se de uma balança
	 * 
	 * @param sender instancia da balança
	 */
	public void onDesconectado(Object sender);
}
