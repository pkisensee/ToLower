__kernel void lower_kernel( __global char* s, size_t len )
{
  int i = get_global_id( 0 );
  if( i >= len )
    return;

  // static const char kCaseOffset = 'a' - 'A'; // from upper to lower
  char c = s[i];
  if( c >= 'A' && c <= 'Z' )
    s[i] = c + 'a' - 'A';
}
