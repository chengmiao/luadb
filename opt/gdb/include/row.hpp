#pragma once 



namespace gdp
{
    namespace db{



        class Row
        {
            public:
                Row(ResultSetSPtr pRes):resultSet(pRes)
            {

            }


            private:
                ResultSetSPtr  resultSet; 
        }; 


    }
}
