#pragma once


namespace MvException
{
	enum class Priority { Info, Warning, FatalError };

	class MovieException : public runtime_error
	{
	public:
		explicit MovieException
		(
			const string& message, 
			const Priority _priorty = Priority::Info, 
			const int line = 0
		)
			: runtime_error(message),
			  
			  file_path("log/log.txt"),
		      priority(_priorty)
		{
			ofstream to_file(file_path, ios_base::out | ios_base::app);
			switch (priority) {
				case Priority::Info:       to_file << "[INFO]: ";        break;
				case Priority::Warning:    to_file << "[WARN]: ";        break;
				case Priority::FatalError: to_file << "[FATAL ERROR]: "; break;
			}
			to_file << message
			        << " At line: "        << line << " in source file."
				    << "| Recorded date: " << rlutil::tmstmp();
			to_file.close();
		}
	private:
		string file_path;
		Priority priority;
	};

	class SchemaException final : public MovieException {
	public:
		explicit SchemaException(const string& message, const Priority _priorty = Priority::Info, const int line = 0)
			: MovieException(message, _priorty, line) {
		}
	};

	class MovieCountryException final : public MovieException {
	public:
		explicit MovieCountryException(const string& message, const Priority _priorty = Priority::Info, const int line = 0)
			: MovieException(message, _priorty, line) {
		}
	};

	class MovieAgeRatingException final : public MovieException {
	public:
		explicit MovieAgeRatingException(const string& message, const Priority _priorty = Priority::Info, const int line = 0)
			: MovieException(message, _priorty, line) {
		}
	};

	class MovieGenreException final : public MovieException {
	public:
		explicit MovieGenreException(const string& message, const Priority _priorty = Priority::Info, const int line = 0)
			: MovieException(message, _priorty, line) {
		}
	};

}









