namespace PasswordHelper.Cripto
{
    internal class SimpleCripto
    {
        public static string Cripto(int key, string input)
        {
            string result = "";
            for (int i = 0; i < input.Length; i++)
            {
                int spin = (key + i) % 95;
                int newValue = (input[i] - 32) + spin;
                newValue = (newValue > 94 ? newValue - 95 : newValue) + 32;
                result = result + (char)newValue;
            }
            return result;
        }

        public static string Decripto(int key, string input)
        {
            string result = "";
            for (int i = 0; i < input.Length; i++)
            {
                int spin = (key + i) % 95;
                int newValue = (input[i] - 32) - spin;
                newValue = (newValue < 0 ? newValue + 95 : newValue) + 32;
                result = result + (char)newValue;
            }
            return result;
        }
    }
}