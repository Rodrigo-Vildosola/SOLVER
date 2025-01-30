import sys
from threading import Semaphore, Lock

class CLIStyles:
    RESET = "\033[0m"
    BOLD = "\033[1m"
    GREEN = "\033[92m"
    BLUE = "\033[94m"
    YELLOW = "\033[93m"
    RED = "\033[91m"
    MAGENTA = "\033[95m"
    CYAN = "\033[96m"

def cli_print(message, level="info"):
    """Formatted CLI print with color coding."""
    styles = {
        "info": CLIStyles.BLUE,
        "success": CLIStyles.GREEN,
        "warning": CLIStyles.YELLOW,
        "error": CLIStyles.RED,
        "step": CLIStyles.CYAN,
    }
    prefix = {
        "info": "[INFO]",
        "success": "[SUCCESS]",
        "warning": "[WARNING]",
        "error": "[ERROR]",
        "step": "[STEP]",
    }
    style = styles.get(level, CLIStyles.RESET)
    cli_message = f"{style}{prefix.get(level, '[INFO]')} {message}{CLIStyles.RESET}"
    print(cli_message)

class ProgressBar:
    def __init__(self, total, prefix='', length=50):
        self.total = total
        self.prefix = prefix
        self.length = length
        self.progress = 0

    def update(self, step=1):
        self.progress += step
        percentage = (self.progress / self.total) * 100
        bar = '█' * int(self.length * self.progress // self.total) + '-' * (self.length - int(self.length * self.progress // self.total))
        sys.stdout.write(f'\r{self.prefix} |{bar}| {self.progress}/{self.total} ({percentage:.1f}%)')
        sys.stdout.flush()

    def finish(self):
        sys.stdout.write(f'\r{self.prefix} |{"█" * self.length}| {self.total}/{self.total} (100.0%)\n')
        sys.stdout.flush()


class FileTracker:
    def __init__(self, total):
        self.total = total
        self.current = 0
        self.lock = Lock()

    def update(self, message):
        """Update the display with the current progress and file message."""
        with self.lock:
            self.current += 1
            sys.stdout.write(f"\r[{self.current}/{self.total}] {message}")
            sys.stdout.flush()

    def skip(self):
        """Increment progress without showing a message."""
        with self.lock:
            self.current += 1

    def finish(self):
        """Finalize the progress display."""
        with self.lock:
            sys.stdout.write("\n")
            sys.stdout.flush()
