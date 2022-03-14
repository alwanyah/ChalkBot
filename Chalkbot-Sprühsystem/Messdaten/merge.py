import aspose.words as aw

def mergepng(fileNames,outname):
    doc = aw.Document()
    builder = aw.DocumentBuilder(doc)

    shapes = [builder.insert_image(fileName) for fileName in fileNames]

    # Calculate the maximum width and height and update page settings 
    # to crop the document to fit the size of the pictures.
    pageSetup = builder.page_setup
    pageSetup.page_width = max(shape.width for shape in shapes)
    pageSetup.page_height = sum(shape.height for shape in shapes)
    pageSetup.top_margin = 0;
    pageSetup.left_margin = 0;
    pageSetup.bottom_margin = 0;
    pageSetup.right_margin = 0;

    doc.save(outname);
    print(str(outname)+" saved")
