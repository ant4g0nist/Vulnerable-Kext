# _ctl_register

Search and get xrefs to `ctl_register failed` and pseudo code should like this:

~~~
   v10 = sub_FFFFFFF008983D28;
    v11 = sub_FFFFFFF008983D44;
    v1 = sub_FFFFFFF007FFC6F4(v4, &unk_FFFFFFF0092E0D80);                               <---- _ctl_register
    v2 = v1;
    if ( v1 )
      sub_FFFFFFF007C29D28("%s - ctl_register failed: %d\n", "en_register", v1);
~~~

- _ctl_register = 0xFFFFFFF007FFC6F4