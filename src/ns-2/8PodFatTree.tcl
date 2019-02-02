source template.tcl

# defining link properties
set edge_link 10.0Gb
set agg_link 10.0Gb
set core_link 10.0Gb
set err_link 5.0Gb

set edge_delay 0.025ms
set agg_delay 0.025ms
set core_delay 0.025ms

set num_hosts 128
set num_nodes 208

set pod 8

# creating nodes
for { set i 0 } { $i <= $num_nodes } { incr i } {
    set n($i) [$ns node]
}

Queue set limit_ 3

set em [new ErrorModel]
$em set rate_ 0
$em drop-target [new Agent/Null]

#set loss_random_variable [new RandomVariable/Uniform] 
#$loss_random_variable set min_ 0 # set the range of the random variable;
#$loss_random_variable set max_ 100
#set em [new ErrorModel]
#$em drop-target [new Agent/Null] 
#$em set rate_ 1
#$em ranvar $loss_random_variable 



# creating links
$ns duplex-link $n(0) $n(128) $edge_link $edge_delay DropTail
$ns duplex-link $n(1) $n(128) $edge_link $edge_delay DropTail
$ns duplex-link $n(2) $n(128) $edge_link $edge_delay DropTail
$ns duplex-link $n(3) $n(128) $edge_link $edge_delay DropTail
$ns duplex-link $n(4) $n(129) $edge_link $edge_delay DropTail
$ns duplex-link $n(5) $n(129) $edge_link $edge_delay DropTail
$ns duplex-link $n(6) $n(129) $edge_link $edge_delay DropTail
$ns duplex-link $n(7) $n(129) $edge_link $edge_delay DropTail
$ns duplex-link $n(8) $n(130) $edge_link $edge_delay DropTail
$ns duplex-link $n(9) $n(130) $edge_link $edge_delay DropTail
$ns duplex-link $n(10) $n(130) $edge_link $edge_delay DropTail
$ns duplex-link $n(11) $n(130) $edge_link $edge_delay DropTail
$ns duplex-link $n(12) $n(131) $edge_link $edge_delay DropTail
$ns duplex-link $n(13) $n(131) $edge_link $edge_delay DropTail
$ns duplex-link $n(14) $n(131) $edge_link $edge_delay DropTail
$ns duplex-link $n(15) $n(131) $edge_link $edge_delay DropTail
$ns duplex-link $n(16) $n(132) $edge_link $edge_delay DropTail
$ns duplex-link $n(17) $n(132) $edge_link $edge_delay DropTail
$ns duplex-link $n(18) $n(132) $edge_link $edge_delay DropTail
$ns duplex-link $n(19) $n(132) $edge_link $edge_delay DropTail
$ns duplex-link $n(20) $n(133) $edge_link $edge_delay DropTail
$ns duplex-link $n(21) $n(133) $edge_link $edge_delay DropTail
$ns duplex-link $n(22) $n(133) $edge_link $edge_delay DropTail
$ns duplex-link $n(23) $n(133) $edge_link $edge_delay DropTail
$ns duplex-link $n(24) $n(134) $edge_link $edge_delay DropTail
$ns duplex-link $n(25) $n(134) $edge_link $edge_delay DropTail
$ns duplex-link $n(26) $n(134) $edge_link $edge_delay DropTail
$ns duplex-link $n(27) $n(134) $edge_link $edge_delay DropTail
$ns duplex-link $n(28) $n(135) $edge_link $edge_delay DropTail
$ns duplex-link $n(29) $n(135) $edge_link $edge_delay DropTail
$ns duplex-link $n(30) $n(135) $edge_link $edge_delay DropTail
$ns duplex-link $n(31) $n(135) $edge_link $edge_delay DropTail
$ns duplex-link $n(32) $n(136) $edge_link $edge_delay DropTail
$ns duplex-link $n(33) $n(136) $edge_link $edge_delay DropTail
$ns duplex-link $n(34) $n(136) $edge_link $edge_delay DropTail
$ns duplex-link $n(35) $n(136) $edge_link $edge_delay DropTail
$ns duplex-link $n(36) $n(137) $edge_link $edge_delay DropTail
$ns duplex-link $n(37) $n(137) $edge_link $edge_delay DropTail
$ns duplex-link $n(38) $n(137) $edge_link $edge_delay DropTail
$ns duplex-link $n(39) $n(137) $edge_link $edge_delay DropTail
$ns duplex-link $n(40) $n(138) $edge_link $edge_delay DropTail
$ns duplex-link $n(41) $n(138) $edge_link $edge_delay DropTail
$ns duplex-link $n(42) $n(138) $edge_link $edge_delay DropTail
$ns duplex-link $n(43) $n(138) $edge_link $edge_delay DropTail
$ns duplex-link $n(44) $n(139) $edge_link $edge_delay DropTail
$ns duplex-link $n(45) $n(139) $edge_link $edge_delay DropTail
$ns duplex-link $n(46) $n(139) $edge_link $edge_delay DropTail
$ns duplex-link $n(47) $n(139) $edge_link $edge_delay DropTail
$ns duplex-link $n(48) $n(140) $edge_link $edge_delay DropTail
$ns duplex-link $n(49) $n(140) $edge_link $edge_delay DropTail
$ns duplex-link $n(50) $n(140) $edge_link $edge_delay DropTail
$ns duplex-link $n(51) $n(140) $edge_link $edge_delay DropTail
$ns duplex-link $n(52) $n(141) $edge_link $edge_delay DropTail
$ns duplex-link $n(53) $n(141) $edge_link $edge_delay DropTail
$ns duplex-link $n(54) $n(141) $edge_link $edge_delay DropTail
$ns duplex-link $n(55) $n(141) $edge_link $edge_delay DropTail
$ns duplex-link $n(56) $n(142) $edge_link $edge_delay DropTail
$ns duplex-link $n(57) $n(142) $edge_link $edge_delay DropTail
$ns duplex-link $n(58) $n(142) $edge_link $edge_delay DropTail
$ns duplex-link $n(59) $n(142) $edge_link $edge_delay DropTail
$ns duplex-link $n(60) $n(143) $edge_link $edge_delay DropTail
$ns duplex-link $n(61) $n(143) $edge_link $edge_delay DropTail
$ns duplex-link $n(62) $n(143) $edge_link $edge_delay DropTail
$ns duplex-link $n(63) $n(143) $edge_link $edge_delay DropTail
$ns duplex-link $n(64) $n(144) $edge_link $edge_delay DropTail
$ns duplex-link $n(65) $n(144) $edge_link $edge_delay DropTail
$ns duplex-link $n(66) $n(144) $edge_link $edge_delay DropTail
$ns duplex-link $n(67) $n(144) $edge_link $edge_delay DropTail
$ns duplex-link $n(68) $n(145) $edge_link $edge_delay DropTail
$ns duplex-link $n(69) $n(145) $edge_link $edge_delay DropTail
$ns duplex-link $n(70) $n(145) $edge_link $edge_delay DropTail
$ns duplex-link $n(71) $n(145) $edge_link $edge_delay DropTail
$ns duplex-link $n(72) $n(146) $edge_link $edge_delay DropTail
$ns duplex-link $n(73) $n(146) $edge_link $edge_delay DropTail
$ns duplex-link $n(74) $n(146) $edge_link $edge_delay DropTail
$ns duplex-link $n(75) $n(146) $edge_link $edge_delay DropTail
$ns duplex-link $n(76) $n(147) $edge_link $edge_delay DropTail
$ns duplex-link $n(77) $n(147) $edge_link $edge_delay DropTail
$ns duplex-link $n(78) $n(147) $edge_link $edge_delay DropTail
$ns duplex-link $n(79) $n(147) $edge_link $edge_delay DropTail
$ns duplex-link $n(80) $n(148) $edge_link $edge_delay DropTail
$ns duplex-link $n(81) $n(148) $edge_link $edge_delay DropTail
$ns duplex-link $n(82) $n(148) $edge_link $edge_delay DropTail
$ns duplex-link $n(83) $n(148) $edge_link $edge_delay DropTail
$ns duplex-link $n(84) $n(149) $edge_link $edge_delay DropTail
$ns duplex-link $n(85) $n(149) $edge_link $edge_delay DropTail
$ns duplex-link $n(86) $n(149) $edge_link $edge_delay DropTail
$ns duplex-link $n(87) $n(149) $edge_link $edge_delay DropTail
$ns duplex-link $n(88) $n(150) $edge_link $edge_delay DropTail
$ns duplex-link $n(89) $n(150) $edge_link $edge_delay DropTail
$ns duplex-link $n(90) $n(150) $edge_link $edge_delay DropTail
$ns duplex-link $n(91) $n(150) $edge_link $edge_delay DropTail
$ns duplex-link $n(92) $n(151) $edge_link $edge_delay DropTail
$ns duplex-link $n(93) $n(151) $edge_link $edge_delay DropTail
$ns duplex-link $n(94) $n(151) $edge_link $edge_delay DropTail
$ns duplex-link $n(95) $n(151) $edge_link $edge_delay DropTail
$ns duplex-link $n(96) $n(152) $edge_link $edge_delay DropTail
$ns duplex-link $n(97) $n(152) $edge_link $edge_delay DropTail
$ns duplex-link $n(98) $n(152) $edge_link $edge_delay DropTail
$ns duplex-link $n(99) $n(152) $edge_link $edge_delay DropTail
$ns duplex-link $n(100) $n(153) $edge_link $edge_delay DropTail
$ns duplex-link $n(101) $n(153) $edge_link $edge_delay DropTail
$ns duplex-link $n(102) $n(153) $edge_link $edge_delay DropTail
$ns duplex-link $n(103) $n(153) $edge_link $edge_delay DropTail
$ns duplex-link $n(104) $n(154) $edge_link $edge_delay DropTail
$ns duplex-link $n(105) $n(154) $edge_link $edge_delay DropTail
$ns duplex-link $n(106) $n(154) $edge_link $edge_delay DropTail
$ns duplex-link $n(107) $n(154) $edge_link $edge_delay DropTail
$ns duplex-link $n(108) $n(155) $edge_link $edge_delay DropTail
$ns duplex-link $n(109) $n(155) $edge_link $edge_delay DropTail
$ns duplex-link $n(110) $n(155) $edge_link $edge_delay DropTail
$ns duplex-link $n(111) $n(155) $edge_link $edge_delay DropTail
$ns duplex-link $n(112) $n(156) $edge_link $edge_delay DropTail
$ns duplex-link $n(113) $n(156) $edge_link $edge_delay DropTail
$ns duplex-link $n(114) $n(156) $edge_link $edge_delay DropTail
$ns duplex-link $n(115) $n(156) $edge_link $edge_delay DropTail
$ns duplex-link $n(116) $n(157) $edge_link $edge_delay DropTail
$ns duplex-link $n(117) $n(157) $edge_link $edge_delay DropTail
$ns duplex-link $n(118) $n(157) $edge_link $edge_delay DropTail
$ns duplex-link $n(119) $n(157) $edge_link $edge_delay DropTail
$ns duplex-link $n(120) $n(158) $edge_link $edge_delay DropTail
$ns duplex-link $n(121) $n(158) $edge_link $edge_delay DropTail
$ns duplex-link $n(122) $n(158) $edge_link $edge_delay DropTail
$ns duplex-link $n(123) $n(158) $edge_link $edge_delay DropTail
$ns duplex-link $n(124) $n(159) $edge_link $edge_delay DropTail
$ns duplex-link $n(125) $n(159) $edge_link $edge_delay DropTail
$ns duplex-link $n(126) $n(159) $edge_link $edge_delay DropTail
$ns duplex-link $n(127) $n(159) $edge_link $edge_delay DropTail
$ns duplex-link $n(128) $n(160) $agg_link $agg_delay DropTail
$ns duplex-link $n(128) $n(161) $agg_link $agg_delay DropTail
$ns duplex-link $n(128) $n(162) $agg_link $agg_delay DropTail
$ns duplex-link $n(128) $n(163) $agg_link $agg_delay DropTail
$ns duplex-link $n(129) $n(160) $agg_link $agg_delay DropTail
$ns duplex-link $n(129) $n(161) $agg_link $agg_delay DropTail
$ns duplex-link $n(129) $n(162) $agg_link $agg_delay DropTail
$ns duplex-link $n(129) $n(163) $agg_link $agg_delay DropTail
$ns duplex-link $n(130) $n(160) $agg_link $agg_delay DropTail
$ns duplex-link $n(130) $n(161) $agg_link $agg_delay DropTail
$ns duplex-link $n(130) $n(162) $agg_link $agg_delay DropTail
$ns duplex-link $n(130) $n(163) $agg_link $agg_delay DropTail
$ns duplex-link $n(131) $n(160) $agg_link $agg_delay DropTail
$ns duplex-link $n(131) $n(161) $agg_link $agg_delay DropTail
$ns duplex-link $n(131) $n(162) $agg_link $agg_delay DropTail
$ns duplex-link $n(131) $n(163) $agg_link $agg_delay DropTail
$ns duplex-link $n(132) $n(164) $agg_link $agg_delay DropTail
$ns duplex-link $n(132) $n(165) $agg_link $agg_delay DropTail
$ns duplex-link $n(132) $n(166) $agg_link $agg_delay DropTail
$ns duplex-link $n(132) $n(167) $agg_link $agg_delay DropTail
$ns duplex-link $n(133) $n(164) $agg_link $agg_delay DropTail
$ns duplex-link $n(133) $n(165) $agg_link $agg_delay DropTail
$ns duplex-link $n(133) $n(166) $agg_link $agg_delay DropTail
$ns duplex-link $n(133) $n(167) $agg_link $agg_delay DropTail
$ns duplex-link $n(134) $n(164) $agg_link $agg_delay DropTail
$ns duplex-link $n(134) $n(165) $agg_link $agg_delay DropTail
$ns duplex-link $n(134) $n(166) $agg_link $agg_delay DropTail
$ns duplex-link $n(134) $n(167) $agg_link $agg_delay DropTail
$ns duplex-link $n(135) $n(164) $agg_link $agg_delay DropTail
$ns duplex-link $n(135) $n(165) $agg_link $agg_delay DropTail
$ns duplex-link $n(135) $n(166) $agg_link $agg_delay DropTail
$ns duplex-link $n(135) $n(167) $agg_link $agg_delay DropTail
$ns duplex-link $n(136) $n(168) $agg_link $agg_delay DropTail
$ns duplex-link $n(136) $n(169) $agg_link $agg_delay DropTail
$ns duplex-link $n(136) $n(170) $agg_link $agg_delay DropTail
$ns duplex-link $n(136) $n(171) $agg_link $agg_delay DropTail
$ns duplex-link $n(137) $n(168) $agg_link $agg_delay DropTail
$ns duplex-link $n(137) $n(169) $agg_link $agg_delay DropTail
$ns duplex-link $n(137) $n(170) $agg_link $agg_delay DropTail
$ns duplex-link $n(137) $n(171) $agg_link $agg_delay DropTail
$ns duplex-link $n(138) $n(168) $agg_link $agg_delay DropTail
$ns duplex-link $n(138) $n(169) $agg_link $agg_delay DropTail
$ns duplex-link $n(138) $n(170) $agg_link $agg_delay DropTail
$ns duplex-link $n(138) $n(171) $agg_link $agg_delay DropTail
$ns duplex-link $n(139) $n(168) $agg_link $agg_delay DropTail
$ns duplex-link $n(139) $n(169) $agg_link $agg_delay DropTail
$ns duplex-link $n(139) $n(170) $agg_link $agg_delay DropTail
$ns duplex-link $n(139) $n(171) $agg_link $agg_delay DropTail
$ns duplex-link $n(140) $n(172) $agg_link $agg_delay DropTail
$ns duplex-link $n(140) $n(173) $agg_link $agg_delay DropTail
$ns duplex-link $n(140) $n(174) $agg_link $agg_delay DropTail
$ns duplex-link $n(140) $n(175) $agg_link $agg_delay DropTail
$ns duplex-link $n(141) $n(172) $agg_link $agg_delay DropTail
$ns duplex-link $n(141) $n(173) $agg_link $agg_delay DropTail
$ns duplex-link $n(141) $n(174) $agg_link $agg_delay DropTail
$ns duplex-link $n(141) $n(175) $agg_link $agg_delay DropTail
$ns duplex-link $n(142) $n(172) $agg_link $agg_delay DropTail
$ns duplex-link $n(142) $n(173) $agg_link $agg_delay DropTail
$ns duplex-link $n(142) $n(174) $agg_link $agg_delay DropTail
$ns duplex-link $n(142) $n(175) $agg_link $agg_delay DropTail
$ns duplex-link $n(143) $n(172) $agg_link $agg_delay DropTail
$ns duplex-link $n(143) $n(173) $agg_link $agg_delay DropTail
$ns duplex-link $n(143) $n(174) $agg_link $agg_delay DropTail
$ns duplex-link $n(143) $n(175) $agg_link $agg_delay DropTail
$ns duplex-link $n(144) $n(176) $agg_link $agg_delay DropTail
$ns duplex-link $n(144) $n(177) $agg_link $agg_delay DropTail
$ns duplex-link $n(144) $n(178) $agg_link $agg_delay DropTail
$ns duplex-link $n(144) $n(179) $agg_link $agg_delay DropTail
$ns duplex-link $n(145) $n(176) $agg_link $agg_delay DropTail
$ns duplex-link $n(145) $n(177) $agg_link $agg_delay DropTail
$ns duplex-link $n(145) $n(178) $agg_link $agg_delay DropTail
$ns duplex-link $n(145) $n(179) $agg_link $agg_delay DropTail
$ns duplex-link $n(146) $n(176) $agg_link $agg_delay DropTail
$ns duplex-link $n(146) $n(177) $agg_link $agg_delay DropTail
$ns duplex-link $n(146) $n(178) $agg_link $agg_delay DropTail
$ns duplex-link $n(146) $n(179) $agg_link $agg_delay DropTail
$ns duplex-link $n(147) $n(176) $agg_link $agg_delay DropTail
$ns duplex-link $n(147) $n(177) $agg_link $agg_delay DropTail
$ns duplex-link $n(147) $n(178) $agg_link $agg_delay DropTail
$ns duplex-link $n(147) $n(179) $agg_link $agg_delay DropTail
$ns duplex-link $n(148) $n(180) $agg_link $agg_delay DropTail
$ns duplex-link $n(148) $n(181) $agg_link $agg_delay DropTail
$ns duplex-link $n(148) $n(182) $agg_link $agg_delay DropTail
$ns duplex-link $n(148) $n(183) $agg_link $agg_delay DropTail
$ns duplex-link $n(149) $n(180) $agg_link $agg_delay DropTail
$ns duplex-link $n(149) $n(181) $agg_link $agg_delay DropTail
$ns duplex-link $n(149) $n(182) $agg_link $agg_delay DropTail
$ns duplex-link $n(149) $n(183) $agg_link $agg_delay DropTail
$ns duplex-link $n(150) $n(180) $agg_link $agg_delay DropTail
$ns duplex-link $n(150) $n(181) $agg_link $agg_delay DropTail
$ns duplex-link $n(150) $n(182) $agg_link $agg_delay DropTail
$ns duplex-link $n(150) $n(183) $agg_link $agg_delay DropTail
$ns duplex-link $n(151) $n(180) $agg_link $agg_delay DropTail
$ns duplex-link $n(151) $n(181) $agg_link $agg_delay DropTail
$ns duplex-link $n(151) $n(182) $agg_link $agg_delay DropTail
$ns duplex-link $n(151) $n(183) $agg_link $agg_delay DropTail
$ns duplex-link $n(152) $n(184) $agg_link $agg_delay DropTail
$ns duplex-link $n(152) $n(185) $agg_link $agg_delay DropTail
$ns duplex-link $n(152) $n(186) $agg_link $agg_delay DropTail
$ns duplex-link $n(152) $n(187) $agg_link $agg_delay DropTail
$ns duplex-link $n(153) $n(184) $agg_link $agg_delay DropTail
$ns duplex-link $n(153) $n(185) $agg_link $agg_delay DropTail
$ns duplex-link $n(153) $n(186) $agg_link $agg_delay DropTail
$ns duplex-link $n(153) $n(187) $agg_link $agg_delay DropTail
$ns duplex-link $n(154) $n(184) $agg_link $agg_delay DropTail
$ns duplex-link $n(154) $n(185) $agg_link $agg_delay DropTail
$ns duplex-link $n(154) $n(186) $agg_link $agg_delay DropTail
$ns duplex-link $n(154) $n(187) $agg_link $agg_delay DropTail
$ns duplex-link $n(155) $n(184) $agg_link $agg_delay DropTail
$ns duplex-link $n(155) $n(185) $agg_link $agg_delay DropTail
$ns duplex-link $n(155) $n(186) $agg_link $agg_delay DropTail
$ns duplex-link $n(155) $n(187) $agg_link $agg_delay DropTail
$ns duplex-link $n(156) $n(188) $agg_link $agg_delay DropTail
$ns duplex-link $n(156) $n(189) $agg_link $agg_delay DropTail
$ns duplex-link $n(156) $n(190) $agg_link $agg_delay DropTail
$ns duplex-link $n(156) $n(191) $agg_link $agg_delay DropTail
$ns duplex-link $n(157) $n(188) $agg_link $agg_delay DropTail
$ns duplex-link $n(157) $n(189) $agg_link $agg_delay DropTail
$ns duplex-link $n(157) $n(190) $agg_link $agg_delay DropTail
$ns duplex-link $n(157) $n(191) $agg_link $agg_delay DropTail
$ns duplex-link $n(158) $n(188) $agg_link $agg_delay DropTail
$ns duplex-link $n(158) $n(189) $agg_link $agg_delay DropTail
$ns duplex-link $n(158) $n(190) $agg_link $agg_delay DropTail
$ns duplex-link $n(158) $n(191) $agg_link $agg_delay DropTail
$ns duplex-link $n(159) $n(188) $agg_link $agg_delay DropTail
$ns duplex-link $n(159) $n(189) $agg_link $agg_delay DropTail
$ns duplex-link $n(159) $n(190) $agg_link $agg_delay DropTail
$ns duplex-link $n(159) $n(191) $agg_link $agg_delay DropTail
$ns duplex-link $n(160) $n(192) $core_link $core_delay DropTail
$ns duplex-link $n(160) $n(193) $core_link $core_delay DropTail
$ns duplex-link $n(160) $n(194) $core_link $core_delay DropTail
$ns duplex-link $n(160) $n(195) $core_link $core_delay DropTail
$ns duplex-link $n(161) $n(196) $core_link $core_delay DropTail
$ns duplex-link $n(161) $n(197) $core_link $core_delay DropTail
$ns duplex-link $n(161) $n(198) $core_link $core_delay DropTail
$ns duplex-link $n(161) $n(199) $core_link $core_delay DropTail
$ns duplex-link $n(162) $n(200) $core_link $core_delay DropTail
$ns duplex-link $n(162) $n(201) $core_link $core_delay DropTail
$ns duplex-link $n(162) $n(202) $core_link $core_delay DropTail
$ns duplex-link $n(162) $n(203) $core_link $core_delay DropTail
$ns duplex-link $n(163) $n(204) $core_link $core_delay DropTail
$ns duplex-link $n(163) $n(205) $core_link $core_delay DropTail
$ns duplex-link $n(163) $n(206) $core_link $core_delay DropTail
$ns duplex-link $n(163) $n(207) $core_link $core_delay DropTail
$ns duplex-link $n(164) $n(192) $core_link $core_delay DropTail
$ns duplex-link $n(164) $n(193) $core_link $core_delay DropTail
$ns duplex-link $n(164) $n(194) $core_link $core_delay DropTail
$ns duplex-link $n(164) $n(195) $core_link $core_delay DropTail
$ns duplex-link $n(165) $n(196) $core_link $core_delay DropTail
$ns duplex-link $n(165) $n(197) $core_link $core_delay DropTail
$ns duplex-link $n(165) $n(198) $core_link $core_delay DropTail
$ns duplex-link $n(165) $n(199) $core_link $core_delay DropTail
$ns duplex-link $n(166) $n(200) $core_link $core_delay DropTail
$ns duplex-link $n(166) $n(201) $core_link $core_delay DropTail
$ns duplex-link $n(166) $n(202) $core_link $core_delay DropTail
$ns duplex-link $n(166) $n(203) $core_link $core_delay DropTail
$ns duplex-link $n(167) $n(204) $core_link $core_delay DropTail
$ns duplex-link $n(167) $n(205) $core_link $core_delay DropTail
$ns duplex-link $n(167) $n(206) $core_link $core_delay DropTail
$ns duplex-link $n(167) $n(207) $core_link $core_delay DropTail
$ns duplex-link $n(168) $n(192) $core_link $core_delay DropTail
$ns duplex-link $n(168) $n(193) $core_link $core_delay DropTail
$ns duplex-link $n(168) $n(194) $core_link $core_delay DropTail
$ns duplex-link $n(168) $n(195) $core_link $core_delay DropTail
$ns duplex-link $n(169) $n(196) $core_link $core_delay DropTail
$ns duplex-link $n(169) $n(197) $core_link $core_delay DropTail
$ns duplex-link $n(169) $n(198) $core_link $core_delay DropTail
$ns duplex-link $n(169) $n(199) $core_link $core_delay DropTail
$ns duplex-link $n(170) $n(200) $core_link $core_delay DropTail
$ns duplex-link $n(170) $n(201) $core_link $core_delay DropTail
$ns duplex-link $n(170) $n(202) $core_link $core_delay DropTail
$ns duplex-link $n(170) $n(203) $core_link $core_delay DropTail
$ns duplex-link $n(171) $n(204) $core_link $core_delay DropTail
$ns duplex-link $n(171) $n(205) $core_link $core_delay DropTail
$ns duplex-link $n(171) $n(206) $core_link $core_delay DropTail
$ns duplex-link $n(171) $n(207) $core_link $core_delay DropTail
$ns duplex-link $n(172) $n(192) $core_link $core_delay DropTail
$ns duplex-link $n(172) $n(193) $core_link $core_delay DropTail
$ns duplex-link $n(172) $n(194) $core_link $core_delay DropTail
$ns duplex-link $n(172) $n(195) $core_link $core_delay DropTail
$ns duplex-link $n(173) $n(196) $core_link $core_delay DropTail
$ns duplex-link $n(173) $n(197) $core_link $core_delay DropTail
$ns duplex-link $n(173) $n(198) $core_link $core_delay DropTail
$ns duplex-link $n(173) $n(199) $core_link $core_delay DropTail
$ns duplex-link $n(174) $n(200) $core_link $core_delay DropTail
$ns duplex-link $n(174) $n(201) $core_link $core_delay DropTail
$ns duplex-link $n(174) $n(202) $core_link $core_delay DropTail
$ns duplex-link $n(174) $n(203) $core_link $core_delay DropTail
$ns duplex-link $n(175) $n(204) $core_link $core_delay DropTail
$ns duplex-link $n(175) $n(205) $core_link $core_delay DropTail
$ns duplex-link $n(175) $n(206) $core_link $core_delay DropTail
$ns duplex-link $n(175) $n(207) $core_link $core_delay DropTail
$ns duplex-link $n(176) $n(192) $core_link $core_delay DropTail
$ns duplex-link $n(176) $n(193) $core_link $core_delay DropTail
$ns duplex-link $n(176) $n(194) $core_link $core_delay DropTail
$ns duplex-link $n(176) $n(195) $core_link $core_delay DropTail
$ns duplex-link $n(177) $n(196) $core_link $core_delay DropTail
$ns duplex-link $n(177) $n(197) $core_link $core_delay DropTail
$ns duplex-link $n(177) $n(198) $core_link $core_delay DropTail
$ns duplex-link $n(177) $n(199) $core_link $core_delay DropTail
$ns duplex-link $n(178) $n(200) $core_link $core_delay DropTail
$ns duplex-link $n(178) $n(201) $core_link $core_delay DropTail
$ns duplex-link $n(178) $n(202) $core_link $core_delay DropTail
$ns duplex-link $n(178) $n(203) $core_link $core_delay DropTail
$ns duplex-link $n(179) $n(204) $core_link $core_delay DropTail
$ns duplex-link $n(179) $n(205) $core_link $core_delay DropTail
$ns duplex-link $n(179) $n(206) $core_link $core_delay DropTail
$ns duplex-link $n(179) $n(207) $core_link $core_delay DropTail
$ns duplex-link $n(180) $n(192) $core_link $core_delay DropTail
$ns duplex-link $n(180) $n(193) $core_link $core_delay DropTail
$ns duplex-link $n(180) $n(194) $core_link $core_delay DropTail
$ns duplex-link $n(180) $n(195) $core_link $core_delay DropTail
$ns duplex-link $n(181) $n(196) $core_link $core_delay DropTail
$ns duplex-link $n(181) $n(197) $core_link $core_delay DropTail
$ns duplex-link $n(181) $n(198) $core_link $core_delay DropTail
$ns duplex-link $n(181) $n(199) $core_link $core_delay DropTail
$ns duplex-link $n(182) $n(200) $core_link $core_delay DropTail
$ns duplex-link $n(182) $n(201) $core_link $core_delay DropTail
$ns duplex-link $n(182) $n(202) $core_link $core_delay DropTail
$ns duplex-link $n(182) $n(203) $core_link $core_delay DropTail
$ns duplex-link $n(183) $n(204) $core_link $core_delay DropTail
$ns duplex-link $n(183) $n(205) $core_link $core_delay DropTail
$ns duplex-link $n(183) $n(206) $core_link $core_delay DropTail
$ns duplex-link $n(183) $n(207) $core_link $core_delay DropTail
$ns duplex-link $n(184) $n(192) $core_link $core_delay DropTail
$ns duplex-link $n(184) $n(193) $core_link $core_delay DropTail
$ns duplex-link $n(184) $n(194) $core_link $core_delay DropTail
$ns duplex-link $n(184) $n(195) $core_link $core_delay DropTail
$ns duplex-link $n(185) $n(196) $core_link $core_delay DropTail
$ns duplex-link $n(185) $n(197) $core_link $core_delay DropTail
$ns duplex-link $n(185) $n(198) $core_link $core_delay DropTail
$ns duplex-link $n(185) $n(199) $core_link $core_delay DropTail
$ns duplex-link $n(186) $n(200) $core_link $core_delay DropTail
$ns duplex-link $n(186) $n(201) $core_link $core_delay DropTail
$ns duplex-link $n(186) $n(202) $core_link $core_delay DropTail
$ns duplex-link $n(186) $n(203) $core_link $core_delay DropTail
$ns duplex-link $n(187) $n(204) $core_link $core_delay DropTail
$ns duplex-link $n(187) $n(205) $core_link $core_delay DropTail
$ns duplex-link $n(187) $n(206) $core_link $core_delay DropTail
$ns duplex-link $n(187) $n(207) $core_link $core_delay DropTail
$ns duplex-link $n(188) $n(192) $core_link $core_delay DropTail
$ns duplex-link $n(188) $n(193) $core_link $core_delay DropTail
$ns duplex-link $n(188) $n(194) $core_link $core_delay DropTail
$ns duplex-link $n(188) $n(195) $core_link $core_delay DropTail
$ns duplex-link $n(189) $n(196) $core_link $core_delay DropTail
$ns duplex-link $n(189) $n(197) $core_link $core_delay DropTail
$ns duplex-link $n(189) $n(198) $core_link $core_delay DropTail
$ns duplex-link $n(189) $n(199) $core_link $core_delay DropTail
$ns duplex-link $n(190) $n(200) $core_link $core_delay DropTail
$ns duplex-link $n(190) $n(201) $core_link $core_delay DropTail
$ns duplex-link $n(190) $n(202) $core_link $core_delay DropTail
$ns duplex-link $n(190) $n(203) $core_link $core_delay DropTail
$ns duplex-link $n(191) $n(204) $core_link $core_delay DropTail
$ns duplex-link $n(191) $n(205) $core_link $core_delay DropTail
$ns duplex-link $n(191) $n(206) $core_link $core_delay DropTail
$ns duplex-link $n(191) $n(207) $core_link $core_delay DropTail




#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


set sim_start [clock seconds]
set link_rate 10
set sim_end 10000
#set load 1.5
#set meanFlowSize 800*1460
set load 3
set meanFlowSize 400*1460
set paretoShape 1.05
set myAgent "Agent/TCP/FullTcp"
Agent/TCP set tcpTick_ 0.000001
Agent/TCP set minrto_ 0.002
Agent/TCP set maxrto_ 64
set debug_mode 0
set lambda [expr ($link_rate*$load*1000000000)/($meanFlowSize*8.0/1460*1500)]

puts "Arrival: Poisson with inter-arrival [expr 1/$lambda * 1000] ms"
puts "FlowSize: Pareto with mean = $meanFlowSize, shape = $paretoShape"

puts "Setting up connections ..."; flush stdout

set flow_gen 0
set flow_fin 0
set init_fid 0
set flow_cdf "CDF.tcl"
set connections_per_pair 1

#build-tcp $n($i) $n($j) 0.2 1.2
set hosts_per_pod [expr $num_hosts/$pod]
for { set i 0 } { $i < $num_hosts-[expr $pod/2 * $pod/2] } { incr i } {
for { set j [expr $i+[expr $hosts_per_pod-[expr $i%$hosts_per_pod]]]} { $j < $num_hosts } {incr j} {
                puts "$i $j"
                set agtagr($i,$j) [new Agent_Aggr_pair]
                $agtagr($i,$j) setup $n($i) $n($j) "$i $j" $connections_per_pair $init_fid "TCP_pair"
				#For Poisson/Pareto
                $agtagr($i,$j) set_PCarrival_process [expr $lambda/($num_hosts - 1)] $flow_cdf [expr 17*$i+1244*$j] [expr 33*$i+4369*$j]
                #$ns at 0.1 "$agtagr($i,$j) warmup 0.5 5"
                $ns at 0.1 "$agtagr($i,$j) init_schedule"
                set init_fid [expr $init_fid + $connections_per_pair];
}
}


for { set i 0 } { $i < 32 } {incr i} {
#$ns at 0.13 "$n([expr 128+$i]) add-route [expr 4*$i]  [$n([expr 160+$i]) entry]"
#$ns rtmodel-at 0.13 down $n([expr 128+$i]) $n([expr 160+$i])
#blackhole
$ns at 0.13 "$n([expr 160+$i]) add-route [expr 4*$i]  [new Agent/Null]"
}   


$ns at 0.15 "finish"
puts "running ns"
$ns run


#$ns rtmodel-at 0.5 down $n(132) $n(16)
#for { set i 0 } { 
#$ns at 0.5 "$n(132) add-route 16 [$n(164) entry]"
