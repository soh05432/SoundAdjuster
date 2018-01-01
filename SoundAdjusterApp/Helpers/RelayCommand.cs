using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Windows.Input;

namespace SoundAdjusterApp.Helpers
{
    public class RelayCommand : ICommand
    {
        private Action<object> _execute;

        private Predicate<object> _canExecute;

        public RelayCommand(Action<object> execute)
            : this(execute, null)
        {

        }

        public RelayCommand(Action<object> execute, Predicate<object> canExecute)
        {
            if (_execute == null)
            {
                throw new ArgumentNullException("execute");
            }

            _execute = execute;
            _canExecute = canExecute;
        }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        public bool CanExecute(object parameter)
        {
            if (_canExecute == null )
            {
                return true;
            }
            else
            {
                return _canExecute(parameter);
            }
        }

        public void Execute(object parameter)
        {
            _execute(parameter);
        }
    }
}
