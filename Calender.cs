// Calender.cs 

using System;
using System.Windows.Forms;



/**********************************************************************************
 *
 *
 *
 */
public class CalendarColumn : DataGridViewColumn {
    public CalendarColumn() : base( new CalendarCell() ) {
    }

    public override DataGridViewCell CellTemplate {
        get {
            return base.CellTemplate;
        }
        set {
            // Ensure that the cell used for the template is a CalendarCell.
            if( value != null &&  !value.GetType().IsAssignableFrom(typeof(CalendarCell)) ) {
                throw new InvalidCastException("Must be a CalendarCell");
            }
            base.CellTemplate = value;
        }
    }
}

/**********************************************************************************
 *
 *
 *
 */
public class DataGridViewNumericUpDownCell : DataGridViewTextBoxCell {
	private int				decimalPlaces;		// Caches the value of the DecimalPlaces;
	private static Type		defaultEditType		= typeof(DataGridViewNumericUpDownEditingControl);
	private static Type		defaultValueType	= typeof(System.Decimal);

	public DataGridViewNumericUpDownCell() {
		this.decimalPlaces	= DATAGRIDVIEWNUMERICUPDOWNCELL_defaultDecimalPlaces;
	}

	[
		DefaultValue(DATAGRIDVIEWNUMERICUPDOWNCELL_defaultDecimalPlaces);
	]

	public int		DecimalPlaces {
		get {
			return this.decimalPlaces;
		}
		set {
			if( value < 0 || value > 99 ){
				throw new ArgumentOutOfRangeException("");
			}

			if( this.decimalPlaces != value ){
				SetDecimalPlaces( this.RowIndex, value );
				OnCommonChange();
			}
		}
	}

	internal void	SetDecimalPlaces( int rowIndex, int value ){
		this.decimalPlaces	= value;
		if( OwnsEditingNumericUpDown(rowIndex) ){
			this.EditingNumericUpDown.DecimalPlaces	= value;
		}
	}

	public override Type	EditType {
		get {
			return defaultEditType;
		}
	}

	public override Type	ValueType {
		get {
			Type valueType = base.ValueType;
			if( valueType != null ){
				return valueType;
			}
			return defaultValueType;
		}
	}
}

/**********************************************************************************
 *
 *
 *
 */
public class CalendarCell : DataGridViewTextBoxCell {
    public CalendarCell() : base() {
        // Use the short date format.
        this.Style.Format	= "d";
    }

    public override void		InitializeEditingControl( int rowIndex, object initialFormattedValue, DataGridViewCellStyle dataGridViewCellStyle ) {
        // Set the value of the editing control to the current cell value.
        base.InitializeEditingControl( rowIndex, initialFormattedValue, dataGridViewCellStyle );
        CalendarEditingControl ctl =  DataGridView.EditingControl as CalendarEditingControl;
        ctl.Value = (DateTime)this.Value;
    }

    public override Type		EditType {
        get {
            // Return the type of the editing contol that CalendarCell uses.
            return typeof(CalendarEditingControl);
        }
    }

    public override Type		ValueType {
        get {
            // Return the type of the value that CalendarCell contains.
            return typeof(DateTime);
        }
    }

    public override object		DefaultNewRowValue {
        get {
            // Use the current date and time as the default value.
            return DateTime.Now;
        }
    }
}

/**********************************************************************************
 *
 *
 *
 */
class CalendarEditingControl : DateTimePicker, IDataGridViewEditingControl {
    DataGridView		dataGridView;
    private bool		valueChanged = false;
    int					rowIndex;

    public CalendarEditingControl() {
        this.Format = DateTimePickerFormat.Short;
    }

    // Implements the IDataGridViewEditingControl.EditingControlFormattedValue 
    // property.
    public object			EditingControlFormattedValue {
        get {
            return this.Value.ToShortDateString();
        }
        set {
            String newValue = value as String;
            if (newValue != null)
            {
                this.Value = DateTime.Parse(newValue);
            }
        }
    }

    // Implements the 
    // IDataGridViewEditingControl.GetEditingControlFormattedValue method.
    public object			GetEditingControlFormattedValue( DataGridViewDataErrorContexts context ) {
        return EditingControlFormattedValue;
    }

    // Implements the 
    // IDataGridViewEditingControl.ApplyCellStyleToEditingControl method.
    public void				ApplyCellStyleToEditingControl( DataGridViewCellStyle dataGridViewCellStyle ) {
        this.Font = dataGridViewCellStyle.Font;
        this.CalendarForeColor = dataGridViewCellStyle.ForeColor;
        this.CalendarMonthBackground = dataGridViewCellStyle.BackColor;
    }

    // Implements the IDataGridViewEditingControl.EditingControlRowIndex 
    // property.
    public int				EditingControlRowIndex {
        get {
            return rowIndex;
        }
        set {
            rowIndex = value;
        }
    }

    // Implements the IDataGridViewEditingControl.EditingControlWantsInputKey 
    // method.
    public bool				EditingControlWantsInputKey( Keys key, bool dataGridViewWantsInputKey ) {
        // Let the DateTimePicker handle the keys listed.
        switch( key & Keys.KeyCode ){
        case Keys.Left:
        case Keys.Up:
        case Keys.Down:
        case Keys.Right:
        case Keys.Home:
        case Keys.End:
        case Keys.PageDown:
        case Keys.PageUp:
            return true;
        default:
            return false;
        }
    }

    // Implements the IDataGridViewEditingControl.PrepareEditingControlForEdit 
    // method.
    public void				PrepareEditingControlForEdit( bool selectAll ) {
        // No preparation needs to be done.
    }

    // Implements the IDataGridViewEditingControl
    // .RepositionEditingControlOnValueChange property.
    public bool				RepositionEditingControlOnValueChange {
        get {
            return false;
        }
    }

    // Implements the IDataGridViewEditingControl
    // .EditingControlDataGridView property.
    public DataGridView		EditingControlDataGridView {
        get {
            return dataGridView;
        }
        set {
            dataGridView = value;
        }
    }

    // Implements the IDataGridViewEditingControl
    // .EditingControlValueChanged property.
    public bool				EditingControlValueChanged	{
        get {
            return valueChanged;
        }
        set {
            valueChanged	= value;
        }
    }

    // Implements the IDataGridViewEditingControl
    // .EditingPanelCursor property.
    public Cursor			EditingPanelCursor {
        get {
            return base.Cursor;
        }
    }

    protected override void	OnValueChanged( EventArgs eventargs ) {
        // Notify the DataGridView that the contents of the cell
        // have changed.
        valueChanged = true;
        this.EditingControlDataGridView.NotifyCurrentCellDirty(true);
        base.OnValueChanged( eventargs );
    }
}

/**********************************************************************************
 *
 *
 *
 */
public class Form1 : Form {
    private DataGridView dataGridView1	= new DataGridView();

    public Form1() {
        this.dataGridView1.Dock		= DockStyle.Fill;
        this.Controls.Add( this.dataGridView1 );
        this.Load += new EventHandler( Form1_Load );
        this.Text = "DataGridView calendar column demo";
    }

    private void	Form1_Load( object sender, EventArgs e ) {
        CalendarColumn	col	= new CalendarColumn();
        this.dataGridView1.Columns.Add( col );
        this.dataGridView1.RowCount	= 5;
        foreach( DataGridViewRow row in this.dataGridView1.Rows ) {
            row.Cells[ 0 ].Value	= DateTime.Now;
        }
    }

    [STAThreadAttribute()]
    public static void Main() {
        Application.Run( new Form1() );
    }
}
