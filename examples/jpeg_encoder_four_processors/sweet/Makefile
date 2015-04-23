.PHONY : dct color_conversion huffman_encode concatenate_huffman_steps read_bitmap merge_and_write clean

clean:
	$(shell find dct color_conversion huffman_encode concatenate_huffman_steps read_bitmap merge_and_write -name '*.alf' -exec rm {} \;)

dct:
	c_to_alf dct/dct
	sweet -i=dct/dct.alf,std_hll.alf func=main annot=dct/dct.ann -ae aac=dct/nios2e.clt tc=st,op merge=all

color_conversion:
	c_to_alf color_conversion/color_conversion
	sweet -i=color_conversion/color_conversion.alf,std_hll.alf func=main annot=color_conversion/color_conversion.ann -ae aac=color_conversion/nios2e.clt tc=st,op merge=all

huffman_encode:
	c_to_alf huffman_encode/huffman_encode
	sweet -i=huffman_encode/huffman_encode.alf,std_hll.alf func=main annot=huffman_encode/huffman_encode.ann -ae aac=huffman_encode/nios2e.clt tc=st,op merge=all

concatenate_huffman_steps:
	c_to_alf concatenate_huffman_steps/concatenate_huffman_steps
	sweet -i=concatenate_huffman_steps/concatenate_huffman_steps.alf,std_hll.alf func=main annot=concatenate_huffman_steps/concatenate_huffman_steps.ann -ae aac=concatenate_huffman_steps/nios2e.clt tc=st,op merge=all

read_bitmap:
	c_to_alf read_bitmap/read_bitmap
	sweet -i=read_bitmap/read_bitmap.alf,std_hll.alf func=main annot=read_bitmap/read_bitmap.ann -ae aac=read_bitmap/nios2e.clt tc=st,op merge=all			

merge_and_write:
	c_to_alf merge_and_write/merge_and_write
	sweet -i=merge_and_write/merge_and_write.alf,std_hll.alf func=main annot=merge_and_write/merge_and_write.ann -ae aac=merge_and_write/nios2e.clt tc=st,op merge=all			
