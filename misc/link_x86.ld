ENTRY(EnterKmMode)
SECTIONS
{
	.text :
	{
		*(.text.EnterKmMode)
		*(.text.*)
	}
}
