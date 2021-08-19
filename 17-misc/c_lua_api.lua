-- funkcja która będziemy wywoływać z C
function jakas_funkcja(val)
	print("To jakaś funkcja", val, tabelka[1])
	return 2*val
end

-- wypisanie tabelki przekazanej z C
print "Wygląd tabelki:"
for index, value in pairs(tabelka) do
	 print(index, value, type(value))
end

-- wywołanie funkcji zdefiniowanej w programie w C
print("Funkcja z C zwróciła:", funkcja_z_c(12, 4))

-- zwrócenie wyników do C
return 23, 20.34, "Ala ma kota"
