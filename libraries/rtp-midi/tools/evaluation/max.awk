// {
  if ( $1 > max ) max = $1
}

END {
  print max
}
