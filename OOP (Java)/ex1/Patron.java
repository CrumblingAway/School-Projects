class Patron 
{
	final String patronFirstName, patronLastName;
	final int comicTendency, dramaticTendency, educationalTendency, patronEnjoymentThreshold;
	
	int patronId;
	
	// Returns literary value of book
	int getBookScore(Book book)
	{
		int bookScore = this.comicTendency*book.bookComicValue+
				this.dramaticTendency*book.bookDramaticValue+
				this.educationalTendency*book.bookEducationalValue;
		return bookScore;
	}
	
	// Returns String of full name of patron
	String stringRepresentation()
	{
		String strRep = this.patronFirstName+" "+this.patronLastName;
		return strRep;
	}
	
	// Returns true if patron will enjoy book, else false
	boolean willEnjoyBook(Book book)
	{
		if(getBookScore(book) < this.patronEnjoymentThreshold)
		{
			return false;
		}
		return true;
	}
	
	// Constructor
	Patron(String patronFirstName, String patronLastName, int comicTendency, int dramaticTendency,
			int educationalTendency, int patronEnjoymentThreshold)
	{
		this.patronFirstName = patronFirstName;
		this.patronLastName = patronLastName;
		this.comicTendency = comicTendency;
		this.dramaticTendency = dramaticTendency;
		this.educationalTendency = educationalTendency;
		this.patronEnjoymentThreshold = patronEnjoymentThreshold;
	}
}
