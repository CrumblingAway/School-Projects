class Book 
{
	final String bookTitle, bookAuthor;
	final int bookYearOfPublication, bookComicValue, bookDramaticValue, 
		bookEducationalValue;
	
	int borrowerId = -1; // Id of borrowing patron, -1 by default
	
	// Returns id of book borrower
	int getCurrentBorrowerId()
	{
		return this.borrowerId;
	}
		
	// Returns literary value of book
	int getLiteraryValue()
	{
		int litVal = this.bookComicValue+this.bookDramaticValue+
				this.bookEducationalValue;
		return litVal;
	}
	
	// Marks book as returned
	void returnBook()
	{
		this.borrowerId = -1;
	}
	
	// Sets given id as book borrower's id, -1 if no borrower
	void setBorrowerId(int borrowerId)
	{
		this.borrowerId = borrowerId;
	}
	
	// Returns String of title, author, publication year, literary value
	String stringRepresentation()
	{
		String strRep = "["+this.bookTitle+","+this.bookAuthor+","+
				this.bookYearOfPublication+","+getLiteraryValue()+"]";
		return strRep;
	}
	
	// Constructor
	Book(String bookTitle, String bookAuthor, int bookYearOfPublication, 
			int bookComicValue, int bookDramaticValue, int bookEducationalValue)
	{
		this.bookTitle = bookTitle;
		this.bookAuthor = bookAuthor;
		this.bookYearOfPublication = bookYearOfPublication;
		this.bookComicValue = bookComicValue;
		this.bookDramaticValue = bookDramaticValue;
		this.bookEducationalValue = bookEducationalValue;
	}
}
