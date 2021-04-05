class Library 
{
	final int maxBookCapacity, maxBorrowedBooks, maxPatronCapacity;
	Book[] bookList; // Database of books
	int bookId = 0; // Counter for book id
	Patron[] patronList; // Database of patrons
	int patronId = 0; // Counter for patron id
	int[] borrowedAmount; // Tracks the amount of books for each patron
	
	// Returns ID of book
	int getBookId(Book book)
	{
		for(int i=0; i<this.bookList.length-1; i++)
		{
			if(this.bookList[i].stringRepresentation()
					.equals(book.stringRepresentation()))
			{
				return i;
			}
		}
		return -1;
	}
	
	// Returns ID of patron
	int getPatronId(Patron patron)
	{
		for(int i=0; i<this.patronList.length; i++)
		{
			if(this.patronList[i]==patron)
			{
				return i;
			}
		}
		return -1;
	}
	
	// Adds book to library, accounting for space and repetition
	int addBookToLibrary(Book book)
	{
		if(!checkBook(book) && this.bookId<=bookList.length-1)
		{
			this.bookList[this.bookId] = book;
			int currentId = this.bookId++;
			return currentId;
		}
		else if(checkBook(book))
		{
			return getBookId(book);
		}
		return -1;
	}
	
	// Check if book is in library
	boolean checkBook(Book book)
	{
		for(Book currentBook: this.bookList)
		{
			
			if(currentBook == null)
			{
				return false;
			}
			else if(currentBook==book)
			{
				return true;
			}
		}
		return false;
	}
	
	boolean checkBookId(int id)
	{
		if(id < 0 || id > this.bookList.length)
		{
			return false;
		}
		return true;
	}
	
	boolean checkPatronId(int id)
	{
		if(id < 0 || id > this.patronList.length)
		{
			return false;
		}
		return true;
	}
	
	// Marks book as borrowed, accounting for borrower and availability
	boolean borrowBook(int bookId, int borrowerId)
	{
		if(!checkBookId(bookId) || !checkPatronId(borrowerId))
		{
			return false;
		}
		if(this.bookList[bookId].getCurrentBorrowerId() == -1 &&
		   this.borrowedAmount[borrowerId] < this.maxBorrowedBooks &&
		   this.patronList[borrowerId].willEnjoyBook(this.bookList[bookId]))
		{
			this.bookList[bookId].setBorrowerId(borrowerId);
			this.borrowedAmount[borrowerId]++;
			return true;
		}
		return false;
	}
	
	// Returns true if book available, else false
	boolean isBookAvailable(int bookId)
	{
		if(bookId > this.bookList.length-1 || this.bookList[bookId] == null)
		{
			return false;
		}
		else if(this.bookList[bookId].getCurrentBorrowerId() == 1)
		{
			return false;
		}
		return true;
	}
	
	// Returns true if book id valid, else false
	boolean isBookIdValid(int bookId)
	{
		if(bookId > this.bookList.length-1 || bookId < 0)
		{
			return false;
		}
		else if(this.bookList[bookId] != null)
		{
			return true;
		}
		return false;
	}
	
	// Returns true if id belongs to a registered patron, else false
	boolean isPatronIdValid(int patronId)
	{
		if(patronId > this.patronList.length-1 || patronId < 0)
		{
			return false;
		}
		else if(this.patronList[patronId] != null)
		{
			return true;
		}
		return false;
	}
	
	// Registers patron if possible, returns new id
	int registerPatronToLibrary(Patron patron)
	{
		if(!checkPatron(patron) && this.patronId <= this.patronList.length-1)
		{
			this.patronList[this.patronId] = patron;
			int currentId = this.patronId++;
			return currentId;
		}
		else if(checkPatron(patron))
		{
			return getPatronId(patron);
		}
		return -1;
	}
	
	boolean checkPatron(Patron patron)
	{
		for(Patron currentPatron: this.patronList)
		{
			
			if(currentPatron == null)
			{
				return false;
			}
			else if(currentPatron==patron)
			{
				return true;
			}
		}
		return false;
	}
	
	// Returns given book
	void returnBook(int bookId)
	{
		if(!checkBookId(bookId))
		{
			return;
		}
		int borrowerId = this.bookList[bookId].getCurrentBorrowerId();
		if(borrowerId==-1)
		{
			return;
		}
		this.borrowedAmount[borrowerId]--;
		this.bookList[bookId].setBorrowerId(-1);
	}
	
	// Suggests book to patron that they will enjoy, if such exist
	Book suggestBookToPatron(int patronId)
	{
		Book tempBook = null;
		int score = this.patronList[patronId].getBookScore(this.bookList[0]);
		for(Book newBook: this.bookList)
		{
			if(this.patronList[patronId].getBookScore(newBook)>=score &&
			   newBook.getCurrentBorrowerId() == -1 && 
			   this.patronList[patronId].willEnjoyBook(newBook))
			{
				tempBook = newBook;
				score = this.patronList[patronId].getBookScore(tempBook); 
			}
		}
		return tempBook;
	}
	
	// Constructor
	Library(int maxBookCapacity, int maxBorrowedBooks, int maxPatronCapacity)
	{
		this.maxBookCapacity = maxBookCapacity;
		this.maxBorrowedBooks = maxBorrowedBooks;
		this.maxPatronCapacity = maxPatronCapacity;
		this.bookList = new Book[this.maxBookCapacity];
		this.patronList = new Patron[this.maxPatronCapacity];
		this.borrowedAmount = new int[this.maxPatronCapacity];
	}
}
