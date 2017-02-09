object Form1: TForm1
  Left = 435
  Top = 300
  BorderStyle = bsDialog
  Caption = 'Obter peso'
  ClientHeight = 321
  ClientWidth = 464
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 40
    Top = 24
    Width = 62
    Height = 29
    Caption = 'Peso:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object LabelPeso: TLabel
    Left = 112
    Top = 24
    Width = 49
    Height = 29
    Caption = '0 Kg'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 28637
    Font.Height = -24
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 40
    Top = 88
    Width = 72
    Height = 29
    Caption = 'Status:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object LabelStatus: TLabel
    Left = 120
    Top = 88
    Width = 155
    Height = 29
    Caption = 'Desconectado'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -24
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label5: TLabel
    Left = 304
    Top = 24
    Width = 70
    Height = 29
    Caption = 'Pre'#231'o:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 40
    Top = 56
    Width = 61
    Height = 29
    Caption = 'Total:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object LabelTotal: TLabel
    Left = 112
    Top = 56
    Width = 81
    Height = 29
    Caption = 'R$ 0,00'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clTeal
    Font.Height = -24
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object ListBox1: TListBox
    Left = 8
    Top = 120
    Width = 449
    Height = 193
    ItemHeight = 13
    TabOrder = 0
  end
  object Button1: TButton
    Left = 352
    Top = 72
    Width = 105
    Height = 41
    Caption = 'Enviar pre'#231'o'
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = Button1Click
  end
  object Edit1: TEdit
    Left = 376
    Top = 24
    Width = 81
    Height = 37
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'Microsoft Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    Text = '3,20'
  end
  object Balanca1: TBalanca
    Ativo = False
    NomeDriver = 'PesoLib.dll'
    OnConectado = Balanca1Conectado
    OnDesconectado = Balanca1Desconectado
    OnPesoRecebido = Balanca1PesoRecebido
    Left = 224
    Top = 24
  end
end
