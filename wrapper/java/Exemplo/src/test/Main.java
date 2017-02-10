package test;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.border.EmptyBorder;

import br.com.mzsw.BalancaListener;
import br.com.mzsw.PesoLib;

public class Main extends JDialog implements BalancaListener, WindowListener {

	/**
	 * 
	 */
	private static final long serialVersionUID = 3285237057515595160L;
	private final JPanel contentPanel = new JPanel();
	private JTextField textField;
	private JList<String> list;
	private PesoLib balanca;
	private JButton btnEnviarPreco;
	private JLabel lblPeso;
	private JLabel lblTotal;
	private JLabel lblStatus;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		try {
			Main dialog = new Main();
			dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
			dialog.setVisible(true);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	static {
		System.loadLibrary("PesoLib");
	}

	/**
	 * Inicializa a conexão com a balança
	 * 
	 */
	private void setupBalanca() {
		// TODO Auto-generated method stub
		balanca = new PesoLib();
		balanca.addEventListener(this);
	}

	/**
	 * Envia o preço do item da balança
	 * 
	 */
	private void actionEnviarPreco() {
		balanca.setPreco(Float.parseFloat(textField.getText().replace(',', '.')));
		// O comando abaixo solicita o peso sem alterar o preço do display da balança
		// balanca.askPeso();
	}
	
	@Override
	public void onConectado(Object arg0) {
		btnEnviarPreco.setEnabled(true);
		lblStatus.setText("Conectado");
		lblStatus.setForeground(new Color(0, 255, 0));
		DefaultListModel<String> model = (DefaultListModel<String>)list.getModel();
		model.addElement("Conectado");
	}

	@Override
	public void onDesconectado(Object arg0) {
		btnEnviarPreco.setEnabled(false);	
		lblStatus.setText("Desconectado");
		lblStatus.setForeground(new Color(255, 0, 0));	
		DefaultListModel<String> model = (DefaultListModel<String>)list.getModel();
		model.addElement("Desconectado");
	}

	@Override
	public void onPesoRecebido(Object arg0, int gramas) {
		String str = String.format("%.3f", gramas / 1000.0) + " kg";
		if(gramas < 1000) {
			str = String.format("%d", gramas) + " g";
		}
		lblPeso.setText(str);
		float total = Float.parseFloat(textField.getText().replace(',', '.'));
		lblTotal.setText("R$ " + String.format("%.2f", (total * gramas / 1000)));
		DefaultListModel<String> model = (DefaultListModel<String>)list.getModel();
		model.addElement("Peso recebido " + str);
	}
	
	@Override
	public void windowClosing(WindowEvent arg0) {
		/* se não fechar a conexão, a aplicação não fecha */
		balanca.fecha();
	}
	
	/**
	 * Create the dialog.
	 */
	public Main() {
		setTitle("Obter peso");
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (ClassNotFoundException | InstantiationException
				| IllegalAccessException | UnsupportedLookAndFeelException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		setBounds(100, 100, 498, 419);
		getContentPane().setLayout(new BorderLayout());
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		contentPanel.setLayout(null);
		
		JLabel lblNewLabel = new JLabel("Peso:");
		lblNewLabel.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 25));
		lblNewLabel.setBounds(41, 34, 70, 38);
		contentPanel.add(lblNewLabel);
		
		JLabel lblTotalLbl = new JLabel("Total:");
		lblTotalLbl.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 25));
		lblTotalLbl.setBounds(41, 73, 70, 38);
		contentPanel.add(lblTotalLbl);
		
		JLabel lblStatusLbl = new JLabel("Status:");
		lblStatusLbl.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 25));
		lblStatusLbl.setBounds(41, 114, 86, 38);
		contentPanel.add(lblStatusLbl);
		
		lblPeso = new JLabel("0 kg");
		lblPeso.setForeground(new Color(255, 127, 80));
		lblPeso.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 25));
		lblPeso.setBounds(118, 34, 148, 38);
		contentPanel.add(lblPeso);
		
		lblTotal = new JLabel("R$ 0,00");
		lblTotal.setForeground(new Color(0, 128, 128));
		lblTotal.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 25));
		lblTotal.setBounds(118, 73, 148, 38);
		contentPanel.add(lblTotal);
		
		lblStatus = new JLabel("Desconectado");
		lblStatus.setForeground(new Color(255, 0, 0));
		lblStatus.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 25));
		lblStatus.setBounds(128, 114, 178, 38);
		contentPanel.add(lblStatus);
		
		textField = new JTextField();
		textField.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 25));
		textField.setText("3,20");
		textField.setBounds(386, 36, 86, 36);
		contentPanel.add(textField);
		textField.setColumns(10);
		
		JLabel lblPreo = new JLabel("Pre\u00E7o:");
		lblPreo.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 25));
		lblPreo.setBounds(298, 34, 80, 38);
		contentPanel.add(lblPreo);
		
		btnEnviarPreco = new JButton("Enviar pre\u00E7o");
		btnEnviarPreco.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				actionEnviarPreco();
			}
		});
		btnEnviarPreco.setEnabled(false);
		btnEnviarPreco.setFont(new Font("Microsoft Sans Serif", Font.PLAIN, 17));
		btnEnviarPreco.setBounds(333, 102, 139, 38);
		contentPanel.add(btnEnviarPreco);
		
		JScrollPane scrollPane = new JScrollPane();
		scrollPane.setBounds(10, 163, 462, 207);
		contentPanel.add(scrollPane);
		
		list = new JList<String>();
	    DefaultListModel<String> listModel = new DefaultListModel<String>();
	    list.setModel(listModel);
		scrollPane.setViewportView(list);
		setLocationRelativeTo(null);
		setupBalanca();
		addWindowListener(this);
	}

	@Override
	public void windowActivated(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowClosed(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}



	@Override
	public void windowDeactivated(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowDeiconified(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowIconified(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void windowOpened(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

}
