"""
Quantum-CSV format reader.
"""

from .circuit import Circuit
from .operation import OPERATIONS

def read_qcsv(src) -> Circuit:
    """
    Product a Circuit from a Q-CSV file.
    """
